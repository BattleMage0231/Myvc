#include <fstream>
#include <memory>
#include "store.h"
#include "errors.h"
#include "hash.h"

using namespace myvc;

bool RepositoryStore::createAt(const fs::path &path) {
    if(fs::exists(path / ".myvc")) return false;
    fs::create_directory(path / ".myvc");
    return true;
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

RepositoryStore::RepositoryStore(fs::path path) : path {std::move(path)} {
    if(!fs::exists(getMyvcPath())) {
        throw not_found {".myvc directory"};
    }
}

template<typename T> std::optional<T> RepositoryStore::load(const fs::path &path) const {
    std::ifstream in {path, std::ios::binary};
    if(!in) return {};
    return T {in, getInstance()};
}

void RepositoryStore::store(const fs::path &path, const Serializable &s) {
    fs::create_directories(path.parent_path());
    std::ofstream out {path, std::ios::binary};
    s.write(out);
}

std::optional<Commit> RepositoryStore::getCommit(Hash h) const {
    return load<Commit>(getObjectPath(h));
}

void RepositoryStore::createCommit(const Commit &c) {
    store(getObjectPath(c.getHash()), c);
}

std::optional<Tree> RepositoryStore::getTree(Hash h) const {
    return load<Tree>(getObjectPath(h));
}

void RepositoryStore::createTree(const Tree &c) {
    store(getObjectPath(c.getHash()), c);
}

std::optional<Blob> RepositoryStore::getBlob(Hash h) const {
    return load<Blob>(getObjectPath(h));
}

void RepositoryStore::createBlob(const Blob &c) {
    store(getObjectPath(c.getHash()), c);
}

std::optional<Branch> RepositoryStore::getBranch(const std::string &name) const {
    return load<Branch>(getBranchPath(name));
}

void RepositoryStore::deleteBranch(const std::string &name) {
    fs::remove(getBranchPath(name));
}

void RepositoryStore::updateBranch(const Branch &branch) {
    store(getBranchPath(branch.getName()), branch);
}

std::optional<Head> RepositoryStore::getHead() const {
    return load<Head>(getHeadPath());
}

void RepositoryStore::updateHead(const Head &h) {
    store(getHeadPath(), h);
}

std::optional<Index> RepositoryStore::getIndex() const {
    return load<Index>(getIndexPath());
}

void RepositoryStore::updateIndex(const Index &i) {
    store(getIndexPath(), i);
}

Blob RepositoryStore::getBlobAt(const fs::path &path) {
    Blob b {{}, getInstance()};
    auto &vec = b.getData();
    std::ifstream in {path, std::ios::binary};
    in >> std::noskipws;
    while(in) {
        char c;
        in >> c;
        vec.emplace_back(c);
    }
    createBlob(b);
    return b;
}

Tree RepositoryStore::getTreeAt(const fs::path &path) {
    std::map<std::string, Tree::Node> nodes;
    for(const auto &entry : fs::directory_iterator(path)) {
        if(entry.path().filename() == ".myvc") continue;
        if(entry.is_directory()) {
            Tree child = getTreeAt(entry.path());
            if(!child.getNodes().empty()) nodes[entry.path().filename()] = Tree::Node {child.getHash(), false};
        } else { 
            nodes[entry.path().filename()] = Tree::Node {getBlobAt(entry.path()).getHash(), true};
        }
    }
    Tree t {nodes, getInstance()};
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

std::optional<Hash> RepositoryStore::resolvePartialObjectHash(const std::string &partial) {
    std::vector<Hash> hashes;
    for(const auto &entry : fs::directory_iterator(getMyvcPath() / "objects")) {
        if(entry.is_regular_file()) {
            std::string entryHash = entry.path().filename().string();
            if(entryHash.starts_with(partial)) {
                hashes.emplace_back(Hash {entryHash});
            }
            if(hashes.size() >= 2) break;
        }
    }
    if(hashes.size() != 1) return {};
    else return hashes[0];
}
