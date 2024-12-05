#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include "cherrypick.h"

using namespace myvc;
using namespace myvc::commands;

Cherrypick::Cherrypick(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Cherrypick::printHelpMessage() {
    std::cerr << "usage: myvc cherry-pick commit" << std::endl;
}

void Cherrypick::process() {
    if(args.size() != 1) throw command_error {"invalid number of arguments"};
    ensureNoUncommitted();
    Head head = resolveHead();
    Commit c = resolveSymbol(args.at(0));
    std::vector<Commit> cParents = c.getParents();
    Tree cTree = cParents.empty() ? Tree {} : cParents[0].getTree();
    // compute three way diff
    TreeDiff diff1 = Tree::diff(cTree, (*head).getTree()), diff2 = Tree::diff(cTree, c.getTree());
    auto res = TreeDiff::merge(diff1, diff2);
    store->setWorkingTree(cTree);
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
    TreeDiff diff = Tree::diff(index.getTree(), (*head).getTree());
    if(diff.getChanges().empty()) {
        std::cout << "Would have no effect." << std::endl;
        return;
    } else if(res.second.deleteConflicts.empty() && res.second.modifyConflicts.empty()) {
        std::set<Hash> parents = {head.getCommit().getHash(), c.getHash()};
        myvc::Commit c { parents, index.getTree().getHash(), std::time(nullptr), "Cherry-pick commit", store };
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
