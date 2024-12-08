#include "repository.h"
#include "treebuilder.h"

using namespace myvc;

Repository::Repository(fs::path path) : path {path}, store {path} {}

Index &Repository::getIndex() const {
    return store.getIndex();
}

Head &Repository::getHead() const {
    return store.getHead();
}

Tree Repository::getWorkingTree() const {
    return store.getWorkingTree();
}

std::optional<std::reference_wrapper<Branch>> Repository::getBranch(const std::string &name) {
    return store.getBranch(name);
}

std::optional<Commit> Repository::getCommit(const Hash &hash) const {
    return store.getCommit(hash);
}

std::optional<Hash> Repository::resolvePartialHash(std::string partial) const {
    return store.resolvePartialObjectHash(std::move(partial));
}

void Repository::addToIndex(const std::vector<fs::path> &paths) {
    Index &index = getIndex();
    TreeBuilder indexBuilder = store.makeTreeBuilder(index.getTree());
    for(const fs::path &p : paths) {
        if(fs::exists(p)) {
            if(fs::is_directory(p)) {
                Tree t = store.getTreeAt(path).value();
                if(t.getNodes().empty()) {
                    indexBuilder.deleteEntry(p);
                } else {
                    indexBuilder.updateEntry(p, Tree::Node {t.hash(), false});
                }
            } else {
                indexBuilder.updateEntry(p, Tree::Node {store.getBlobAt(path).value().hash(), true});
            }
        } else {
            indexBuilder.deleteEntry(p);
        }
    }
    index.setTree(indexBuilder.getTree().hash());
}

void Repository::commitIndex(std::string msg, std::set<Hash> otherParents) {
    Index &index = store.getIndex();
    Head &head = store.getHead();
    time_t time = std::time(nullptr);
    if(head.hasState()) {
        otherParents.insert(head.getCommit().hash());
    }
    Commit c { std::move(otherParents), index.getTree().hash(), time, std::move(msg) };
    store.createCommit(c);
    if(head.hasState()) {
        auto val = head.get();
        if(std::holds_alternative<std::reference_wrapper<Branch>>(val)) {
            Branch &b = std::get<std::reference_wrapper<Branch>>(val);
            b.setCommit(c.hash());
        } else if(std::holds_alternative<Commit>(val)) {
            head.setCommit(c.hash());
        }
    } else {
        store.createBranch(defaultBranch, c.hash());
        head.setBranch(defaultBranch);
    }
}
