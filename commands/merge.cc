#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include "merge.h"

using namespace myvc;
using namespace myvc::commands;

Merge::Merge(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Merge::printHelpMessage() {
    std::cerr << "usage: myvc merge commit" << std::endl;
}

void Merge::process() {
    if(args.size() != 1) throw command_error {"invalid number of arguments"};
    ensureNoUncommitted();
    Head head = resolveHead();
    Commit c = resolveSymbol(args.at(0));
    auto lca = Commit::getLCA(*head, c);
    if(!lca) throw command_error {"no lca exists between the two commits"};
    Commit lcaCommit = lca.value();
    // compute three way diff
    TreeDiff diff1 = Tree::diff(lcaCommit.getTree(), (*head).getTree()), diff2 = Tree::diff(lcaCommit.getTree(), c.getTree());
    if(diff1.getChanges().empty()) {
        std::cout << "Already up to date." << std::endl;
    } else if(diff2.getChanges().empty()) {
        std::cout << "Fast forwarding..." << std::endl;
        if(head.getBranch()) {
            Branch b = head.getBranch().value();
            b.setCommit(c);
            b.store();
        } else {
            head.setState(c);
            head.store();
        }
    } else {
        auto res = TreeDiff::merge(diff1, diff2);
        // stage changes
        for(const auto &[path, change] : res.first.getChanges()) {
            if(change.type == TreeChange::Type::Add || change.type == TreeChange::Type::Modify) {
                fs::create_directories(path.parent_path());
                std::ofstream out {path};
                change.newBlob.write(out);
            } else {
                fs::remove(path);
            }
        }
        Index index = resolveIndex();
        index.updateTree(store->getWorkingTree().getHash());
        if(res.second.deleteConflicts.empty() && res.second.modifyConflicts.empty()) {
            std::set<Hash> parents = {head.getCommit().getHash(), c.getHash()};
            myvc::Commit c { parents, index.getTree().getHash(), std::time(nullptr), "Merge commit", store };
            c.store();
            if(head.isBranch()) {
                Branch b = head.getBranch().value();
                b.setCommit(c.getHash());
                b.store();
            } else {
                head.setState(c);
                head.store();
            }
            index.reset(index.getTree().getHash());
        } else {
            auto conflicts = res.second;
            const auto &changes1 = diff1.getChanges(), &changes2 = diff2.getChanges();
            for(const auto &path : conflicts.deleteConflicts) {
                std::cout << "CONFLICT: " << path << " (using ";
                if(changes1.find(path) != changes1.end()) {
                    std::cout << "HEAD";
                } else {
                    fs::create_directories(path.parent_path());
                    std::ofstream out {path};
                    changes2.at(path).newBlob.write(out);
                    std::cout << "other";
                }
                std::cout << "'s version" << std::endl;
            }
            for(const auto &[path, tried] : conflicts.modifyConflicts) {
                std::vector<Hunk> hunks;
                for(const auto &[conf1, conf2] : tried.second) {
                    std::vector<Change> changes;
                    for(const auto &line : conf1.getOurs()) {
                        changes.emplace_back(Change { Change::Type::Delete, line });
                    }
                    changes.emplace_back(Change { Change::Type::Add, "<<<<<<< HEAD" });
                    for(const auto &line : conf1.getTheirs()) {
                        changes.emplace_back(Change { Change::Type::Add, line });
                    }
                    changes.emplace_back(Change { Change::Type::Add, "=======" });
                    for(const auto &line : conf2.getTheirs()) {
                        changes.emplace_back(Change { Change::Type::Add, line });
                    }
                    changes.emplace_back(Change { Change::Type::Add, ">>>>>>> other" });
                    hunks.emplace_back(Hunk {conf1.getIndex(), std::move(changes)});
                }
                Diff d { tried.first.getBase(), hunks };
                Diff merged = Diff::merge(tried.first, d).first;
                Blob b {merged.apply()};
                fs::create_directories(path.parent_path());
                std::ofstream out {path};
                b.write(out);
                std::cout << "CONFLICT: " << path << " (conflict markers inserted)" << std::endl;
            }
            std::cout << "Merge conflict(s) detected." << std::endl;
        }
    }
}
