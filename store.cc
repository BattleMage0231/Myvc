#include <fstream>
#include <memory>
#include "store.h"
#include "errors.h"
#include "hash.h"

using namespace myvc;

void RepositoryStore::createAt(const fs::path &path) {
    fs::create_directory(path / ".myvc");
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
    return T {in, std::make_unique<RepositoryStore>(*this)};
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

Tree RepositoryStore::getWorkingTree() const {
    throw not_implemented {};
}

void RepositoryStore::setWorkingTree(const Tree &) {
    throw not_implemented {};
}

Hash RepositoryStore::resolvePartialHash(const std::string &) {
    throw not_implemented {};
}
