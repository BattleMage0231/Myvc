#include <fstream>
#include <memory>
#include <concepts>
#include <exception>
#include "store.h"
#include "errors.h"
#include "hash.h"
#include "fileops.h"

using namespace myvc;

bool RepositoryStore::existsAt(const fs::path &path) {
    return fs::exists(path / myvcName);
}

bool RepositoryStore::createAt(const fs::path &path) {
    if(existsAt(path)) return false;
    fs::create_directory(path / myvcName);
    return true;
}

RepositoryStore::RepositoryStore(fs::path repoPath) : repoPath {std::move(repoPath)} {
    if(!fs::exists(getMyvcPath())) {
        THROW(".myvc does not exist");
    }
    self = std::shared_ptr<RepositoryStore>(this, [](auto *) {});
}

std::shared_ptr<RepositoryStore> RepositoryStore::getInstance() const {
    return self;
}

fs::path RepositoryStore::getMyvcPath() const {
    return repoPath / myvcName;
}

fs::path RepositoryStore::getObjectPath(const Hash &h) const {
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

template<typename T> concept HasProvider = requires(T t, std::weak_ptr<typename T::Provider> provider) {
    t.setProvider(provider);
};

template<typename T> std::optional<T> RepositoryStore::load(const fs::path &path) const {
    std::ifstream in {path, std::ios::binary};
    if(!in) return {};
    T t;
    t.read(in);
    if constexpr(HasProvider<T>) {
        t.setProvider(getInstance());
    }
    return t;
}

void RepositoryStore::store(const fs::path &path, const Serializable &s) {
    fs::create_directories(path.parent_path());
    std::ofstream out {path, std::ios::binary};
    s.write(out);
}

template<typename T> bool RepositoryStore::createObject(T &o) {
    Hash h = o.hash();
    if(objects.find(h) == objects.end()) {
        auto ptr = std::make_unique<T>(o);
        if constexpr(HasProvider<T>) {
            o.setProvider(getInstance());
            ptr->setProvider(getInstance());
        }
        objects.insert_or_assign(h, std::move(ptr)); 
        return true;
    }
    if constexpr(HasProvider<T>) o.setProvider(getInstance());
    return false;
}

template<typename T> std::optional<T> RepositoryStore::loadObject(const Hash &h) const {
    if(objects.find(h) != objects.end()) return static_cast<T &>(*objects.at(h));
    auto val = load<T>(getObjectPath(h));
    if(!val) return {};
    objects.insert_or_assign(h, std::make_unique<T>(std::move(val.value())));
    return static_cast<T &>(*objects.at(h));
}

bool RepositoryStore::createCommit(Commit &c) {
    return createObject<Commit>(c);
}

bool RepositoryStore::createTree(Tree &c) {
    return createObject<Tree>(c);
}

bool RepositoryStore::createBlob(Blob &c) {
    return createObject<Blob>(c);
}

const fs::path &RepositoryStore::getPath() const {
    return repoPath;
}

bool RepositoryStore::createBranch(std::string name, Hash commitHash) {
    if(branches.find(name) == branches.end()) {
        Branch b { name, std::move(commitHash) };
        b.setProvider(getInstance());
        branches.insert_or_assign(name, std::move(b));
        return true;
    }
    return false;
}

std::optional<Commit> RepositoryStore::getCommit(const Hash &h) const {
    return loadObject<Commit>(h);
}

std::optional<Tree> RepositoryStore::getTree(const Hash &h) const {
    return loadObject<Tree>(h);
}

std::optional<Blob> RepositoryStore::getBlob(const Hash &h) const {
    return loadObject<Blob>(h);
}

std::optional<std::reference_wrapper<Branch>> RepositoryStore::getBranch(const std::string &name) {
    if(branches.find(name) != branches.end()) return branches.at(name);
    auto val = load<Branch>(getBranchPath(name));
    if(!val) return {};
    branches.insert_or_assign(name, std::move(val.value()));
    return branches.at(name);
}

std::optional<const std::reference_wrapper<Branch>> RepositoryStore::getBranch(const std::string &name) const {
    auto res = const_cast<RepositoryStore &>(*this).getBranch(name);
    if(res) return res.value();
    else return {};
}

std::vector<std::reference_wrapper<Branch>> RepositoryStore::getAllBranches() {
    std::vector<std::reference_wrapper<Branch>> res;
    for(const auto &entry : fs::directory_iterator(getMyvcPath() / "refs" / "heads")) {
        if(fs::is_regular_file(entry.status())) {
            res.emplace_back(getBranch(entry.path().filename()).value());
        }
    }
    return res;
}

Index &RepositoryStore::getIndex() {
    if(index) return index.value();
    auto val = load<Index>(getIndexPath());
    if(!val) {
        Tree t;
        createTree(t);
        index = Index {t.hash()};
        index.value().setProvider(getInstance());
    } else {
        index = val.value();
    }
    return index.value();
}

Head &RepositoryStore::getHead() {
    if(head) return head.value();
    auto val = load<Head>(getHeadPath());
    if(!val) {
        head = Head {};
        head.value().setProvider(getInstance());
    } else {
        head = val.value();
    }
    return head.value();
}

bool RepositoryStore::deleteBranch(const std::string &branch) {
    fs::path branchPath = getBranchPath(branch);
    bool deleted = false;
    if(fs::exists(branchPath)) {
        fileops::remove(branchPath);
        deleted = true;
    }
    if(branches.find(branch) != branches.end()) {
        branches.erase(branch);
        deleted = true;
    }
    return deleted;
}

TreeBuilder RepositoryStore::makeTreeBuilder(Tree t) {
    t.setProvider(getInstance());
    return TreeBuilder {getInstance(), t};
}

std::optional<Blob> RepositoryStore::getBlobAt(const fs::path &path) {
    std::vector<char> vec;
    std::ifstream in {path, std::ios::binary};
    if(!in) return {};
    in.seekg(0, std::ios::end);
    size_t sz = in.tellg();
    in.seekg(0, std::ios::beg);
    vec.resize(sz);
    in.read(vec.data(), sz);
    Blob b {vec};
    createBlob(b);
    return b;
}

std::optional<Tree> RepositoryStore::getTreeAt(const fs::path &path) {
    std::map<std::string, Tree::Node> nodes;
    if(!fs::is_directory(path)) return {};
    for(const auto &entry : fs::directory_iterator(path)) {
        auto name = entry.path().filename();
        if(name == ".myvc" || name == "." || name == "..") continue;
        if(entry.is_directory()) {
            Tree child = getTreeAt(entry.path()).value();
            if(!child.getNodes().empty()) nodes[entry.path().filename()] = Tree::Node {child.hash(), false};
        } else { 
            nodes[entry.path().filename()] = Tree::Node {getBlobAt(entry.path()).value().hash(), true};
        }
    }
    Tree t {std::move(nodes)};
    createTree(t);
    return t;
}

Tree RepositoryStore::getWorkingTree() {
    if(workingTree) return getTree(workingTree.value()).value();
    Tree t = getTreeAt(repoPath).value();
    workingTree = t.hash();
    return t;
}

void RepositoryStore::storeWorkingTree() {
    if(!workingTree) return;
    Tree cur = getTreeAt(repoPath).value();
    Tree next = getWorkingTree();
    TreeDiff diff = Tree::diff(cur, next);
    for(const auto &[path, change] : diff) {
        if(change.type == TreeChange::Type::Add || change.type == TreeChange::Type::Modify) {
            debug_log("writing to " + static_cast<std::string>(path));
            if(!path.parent_path().empty()) fs::create_directories(path.parent_path());
            auto data = change.newBlob.getData();
            std::ofstream out {path, std::ios::binary};
            out.write(data.data(), data.size());
        } else {
            debug_log("deleting " + static_cast<std::string>(path));
            fileops::remove(path);
            for(fs::path par = path.parent_path(); par != "." && fs::is_directory(par) && fs::is_empty(par); par = par.parent_path()) {
                fileops::remove(par);
            }
        }
    }
}

void RepositoryStore::applyOnWorkingTree(const TreeDiff &diff) {
    TreeBuilder builder = makeTreeBuilder(getWorkingTree());
    for(const auto &[path, change] : diff) {
        if(change.type == TreeChange::Type::Add || change.type == TreeChange::Type::Modify) {
            Blob newBlob = change.newBlob;
            createBlob(newBlob);
            Tree::Node node {newBlob.hash(), true};
            node.setProvider(getInstance());
            builder.updateEntry(path, std::move(node));
        } else {
            builder.deleteEntry(path);
        }
    }
    workingTree = builder.getTree().hash();
}

void RepositoryStore::setWorkingTree(const Tree &tree) {
    workingTree = tree.hash();
}

std::optional<Hash> RepositoryStore::resolvePartialHash(const std::string &partial) {
    std::vector<Hash> hashes;
    if(!fs::exists(getMyvcPath() / "objects")) return {};
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
    else return hashes.at(0);
}

RepositoryStore::~RepositoryStore() {
    if(std::uncaught_exceptions() > 0) {
        debug_log("exception detected, state not being stored");
        return;
    }
    if(workingTree) storeWorkingTree();
    if(index) store(getIndexPath(), index.value());
    if(head && head.value().hasState()) store(getHeadPath(), head.value());
    for(const auto &[h, optr] : objects) store(getObjectPath(h), *optr);
    for(const auto &[name, b] : branches) store(getBranchPath(name), b);
}
