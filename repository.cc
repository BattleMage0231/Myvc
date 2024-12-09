#include "repository.h"
#include "treebuilder.h"

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
