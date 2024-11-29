#include <fstream>
#include <memory>
#include "store.h"
#include "errors.h"
#include "hash.h"

using namespace myvc;

void RepositoryStore::createAt(const fs::path &path) {
    fs::create_directory(path / ".myvc");
}

std::shared_ptr<RepositoryStore> RepositoryStore::getInstance() const {
    return std::make_shared<RepositoryStore>(*this);
}

fs::path RepositoryStore::getMyvcPath() const {
    return path / ".myvc";
}

fs::path RepositoryStore::getObjectPath(Hash h) const {
    return getMyvcPath() / "objects" / static_cast<std::string>(h);
}

fs::path RepositoryStore::getBranchPath(const std::string &name) const {
    return getMyvcPath() / "refs" / "heads" / name;
}

fs::path RepositoryStore::getHeadPath() const {
    return getMyvcPath() / "HEAD";
}

fs::path RepositoryStore::getIndexPath() const {
    return getMyvcPath() / "index";
}

RepositoryStore::RepositoryStore(fs::path path) : path {std::move(path)} {}

template<typename T> T RepositoryStore::load(const fs::path &path) const {
    std::ifstream in {path, std::ios::binary};
    std::shared_ptr<RepositoryStore> copy;
    return T {in, getInstance()};
}

void RepositoryStore::store(const fs::path &path, const Serializable &s) {
    std::ofstream out {path, std::ios::binary};
    s.write(out);
}

Commit RepositoryStore::getCommit(Hash h) const {
    return load<Commit>(getObjectPath(h));
}

void RepositoryStore::createCommit(const Commit &c) {
    store(getObjectPath(c.getHash()), c);
}

Tree RepositoryStore::getTree(Hash h) const {
    return load<Tree>(getObjectPath(h));
}

void RepositoryStore::createTree(const Tree &c) {
    store(getObjectPath(c.getHash()), c);
}

Blob RepositoryStore::getBlob(Hash h) const {
    return load<Blob>(getObjectPath(h));
}

void RepositoryStore::createBlob(const Blob &c) {
    store(getObjectPath(c.getHash()), c);
}

Branch RepositoryStore::getBranch(const std::string &name) const {
    return load<Branch>(getBranchPath(name));
}

void RepositoryStore::deleteBranch(const std::string &name) {
    fs::remove(getBranchPath(name));
}

void RepositoryStore::updateBranch(const Branch &branch) {
    store(getBranchPath(branch.getName()), branch);
}

Head RepositoryStore::getHead() const {
    return load<Head>(getHeadPath());
}

void RepositoryStore::updateHead(const Head &h) {
    store(getHeadPath(), h);
}

Index RepositoryStore::getIndex() const {
    return load<Index>(getHeadPath());
}

void RepositoryStore::updateIndex(const Index &i) {
    store(getIndexPath(), i);
}

Tree RepositoryStore::getTreeAt(const fs::path &path) {
    Tree t {{}, getInstance()};
    auto nodes = t.getNodes();
    for(const auto &entry : fs::directory_iterator(path)) {
        if(entry.path().filename() == ".myvc") continue;
        Tree::Node node;
        if(entry.is_directory()) {
            node.setTree(getTreeAt(entry.path()).getHash());
        } else {
            Blob b {{}, getInstance()};
            auto vec = b.getData();
            std::ifstream in {entry.path(), std::ios::binary};
            while(in) {
                char c;
                in >> c;
                vec.push_back(c);
            }
            createBlob(b);
            node.setBlob(b.getHash());
        }
        nodes[entry.path().filename()] = node;
    }
    createTree(t);
    return t;
}

Tree RepositoryStore::getWorkingTree() {
    return getTreeAt(path);
}

void RepositoryStore::setTreeAt(const fs::path &path, const Tree &tree) {
    throw not_implemented {};
}

void RepositoryStore::setWorkingTree(const Tree &) {
    // compute the diff, and then write the files as needed
    throw not_implemented {};
}

Hash RepositoryStore::resolvePartialObjectHash(const std::string &) {
    throw not_implemented {};
}
