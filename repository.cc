#include "repository.h"
#include "treebuilder.h"
#include "errors.h"

using namespace myvc;

Repository::Repository(fs::path path) : RepositoryStore {std::move(path)} {}

void Repository::addToIndex(const std::vector<fs::path> &paths) {
    Index &index = getIndex();
    TreeBuilder indexBuilder = makeTreeBuilder(index.getTree());
    for(const fs::path &p : paths) {
        if(fs::exists(p)) {
            if(fs::is_directory(p)) {
                Tree t = getTreeAt(p).value();
                if(t.getNodes().empty()) {
                    indexBuilder.deleteEntry(p);
                } else {
                    indexBuilder.updateEntry(p, Tree::Node {t.hash(), false});
                }
            } else {
                indexBuilder.updateEntry(p, Tree::Node {getBlobAt(p).value().hash(), true});
            }
        } else {
            indexBuilder.deleteEntry(p);
        }
    }
    index.setTree(indexBuilder.getTree().hash());
}

void Repository::removeFromIndex(const std::vector<fs::path> &paths, bool cached) {
    Index &index = getIndex();
    TreeBuilder indexBuilder = makeTreeBuilder(index.getTree());
    for(const fs::path &p : paths) {
        indexBuilder.deleteEntry(p);
        if(!cached) fs::remove_all(p);
    }
    index.setTree(indexBuilder.getTree().hash());
}

void Repository::commitIndex(std::string msg, std::set<Hash> otherParents) {
    Index &index = getIndex();
    Head &head = getHead();
    time_t time = std::time(nullptr);
    if(head.hasState()) {
        otherParents.insert(head.getCommit().hash());
    }
    Commit c { std::move(otherParents), index.getTree().hash(), time, std::move(msg) };
    createCommit(c);
    if(head.hasState()) {
        moveHeadSticky(c.hash());
    } else {
        createBranch(defaultBranch, c.hash());
        head.setBranch(defaultBranch);
    }
}

void Repository::moveHeadSticky(const Hash &hash) {
    Head &head = getHead();
    auto val = head.get();
    if(std::holds_alternative<std::reference_wrapper<Branch>>(val)) {
        Branch &b = std::get<std::reference_wrapper<Branch>>(val);
        b.setCommit(hash);
    } else {
        head.setCommit(hash);
    }
}

void Repository::checkout(const std::string &branch) {
    Index &index = getIndex();
    Head &head = getHead();
    Branch &b = getBranch(branch).value();
    index.setTree(b.getCommit().getTree().hash());
    head.setBranch(b.getName());
    setWorkingTree(b.getCommit().getTree());
}

std::vector<fs::path> Repository::merge(const Hash &hash) {
    Commit headCommit = getHead().getCommit();
    Commit c = getCommit(hash).value();
    Commit lca = Commit::getLCA(headCommit, c);
    TreeDiff diff1 = Tree::diff(lca.getTree(), headCommit.getTree());
    TreeDiff diff2 = Tree::diff(lca.getTree(), c.getTree());
    auto res = TreeDiff::merge(diff1, diff2);
    setWorkingTree(lca.getTree());
    applyOnWorkingTree(res.first);
    Index &index = getIndex();
    index.setTree(getWorkingTree().hash());
    TreeDiff::Conflicts conflicts = res.second;
    if(conflicts.deleteConflicts.empty() && conflicts.modifyConflicts.empty()) {
        commitIndex("Merge commit", {c.hash()});
        return {};
    } else {
        std::vector<fs::path> conflicted;
        const auto &changes1 = diff1.getChanges(), &changes2 = diff2.getChanges();
        std::map<fs::path, TreeChange> changes;
        for(const auto &path : conflicts.deleteConflicts) {
            bool in1 = changes1.find(path) != changes1.end();
            bool in2 = changes2.find(path) != changes2.end();
            if(in1 && in2) {
                Blob newBlob;
                if(changes1.at(path).type == TreeChange::Type::Delete) {
                    newBlob = changes2.at(path).newBlob;
                } else {
                    newBlob = changes1.at(path).newBlob;
                }
                changes.insert_or_assign(path, TreeChange {TreeChange::Type::Modify, changes1.at(path).oldBlob, newBlob});
            }
            conflicted.emplace_back(path);
        }
        for(const auto &[path, tried] : conflicts.modifyConflicts) {
            std::vector<Hunk> hunks;
            for(const auto &[conf1, conf2] : tried.second) {
                std::vector<Change> curChanges;
                for(const auto &line : conf1.getOurs()) {
                    curChanges.emplace_back(Change { Change::Type::Delete, line });
                }
                curChanges.emplace_back(Change { Change::Type::Add, "<<<<<<< HEAD" });
                for(const auto &line : conf1.getTheirs()) {
                    curChanges.emplace_back(Change { Change::Type::Add, line });
                }
                curChanges.emplace_back(Change { Change::Type::Add, "=======" });
                for(const auto &line : conf2.getTheirs()) {
                    curChanges.emplace_back(Change { Change::Type::Add, line });
                }
                curChanges.emplace_back(Change { Change::Type::Add, ">>>>>>> other" });
                hunks.emplace_back(Hunk {conf1.index, std::move(curChanges)});
            }
            Diff d { tried.first.getBase(), hunks };
            auto mergedWithConf = Diff::merge(tried.first, d);
            if(mergedWithConf.second.empty()) {
                Blob b {mergedWithConf.first.apply()};
                createBlob(b);
                changes.insert_or_assign(path, TreeChange {TreeChange::Type::Modify, changes1.at(path).oldBlob, b});
            } else {
                THROW("unexpected conflicts while merging blobs");
            }
            conflicted.emplace_back(path);
        }
        applyOnWorkingTree(TreeDiff {changes});
        return conflicted;
    }
}
