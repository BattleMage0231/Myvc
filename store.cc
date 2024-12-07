#include <fstream>
#include <memory>
#include <concepts>
#include "store.h"
#include "errors.h"
#include "hash.h"

using namespace myvc;

bool RepositoryStore::createAt(const fs::path &path) {
    if(fs::exists(path / myvcName)) return false;
    fs::create_directory(path / myvcName);
    return true;
}

RepositoryStore::RepositoryStore(fs::path path) : path {std::move(path)} {
    if(!fs::exists(getMyvcPath())) {
        THROW(".myvc does not exist");
    }
}

std::shared_ptr<RepositoryStore> RepositoryStore::getInstance() const {
    try {
        return std::const_pointer_cast<RepositoryStore>(shared_from_this());
    } catch(...) {
        return std::shared_ptr<RepositoryStore>(const_cast<RepositoryStore *>(this), [](auto *) {});
    }
}

fs::path RepositoryStore::getMyvcPath() const {
    return path / myvcName;
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
    if constexpr(HasProvider<T>) t.setProvider(getInstance());
    return t;
}

void RepositoryStore::store(const fs::path &path, const Serializable &s) {
    fs::create_directories(path.parent_path());
    std::ofstream out {path, std::ios::binary};
    s.write(out);
}

template<typename T> bool RepositoryStore::createObject(T &o) {
    Hash h = o.hash();
    if(objects.find(o) == objects.end()) {
        auto ptr = std::make_unique<T>(o);
        if constexpr(HasProvider<T>) ptr->setProvider(getInstance());
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
    objects.at(h) = std::make_unique<T>(std::move(val.value()));
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
    branches.at(name) = std::move(val.value());
    return branches.at(name);
}

std::optional<const std::reference_wrapper<Branch>> RepositoryStore::getBranch(const std::string &name) const {
    auto res = const_cast<RepositoryStore &>(*this).getBranch(name);
    if(res) return res.value();
    else return {};
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

bool RepositoryStore::deleteBranch(const Branch &b) {
    fs::path branchPath = getBranchPath(b.getName());
    bool deleted = false;
    if(fs::exists(branchPath)) {
        fs::remove(branchPath);
        deleted = true;
    }
    if(branches.find(b.getName()) != branches.end()) {
        branches.erase(b.getName());
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
    in >> std::noskipws;
    while(in) {
        char c;
        in >> c;
        vec.emplace_back(c);
    }
    Blob b {vec};
    createBlob(b);
    return b;
}

std::optional<Tree> RepositoryStore::getTreeAt(const fs::path &path) {
    std::map<std::string, Tree::Node> nodes;
    if(!fs::is_directory(path)) return {};
    for(const auto &entry : fs::directory_iterator(path)) {
        if(entry.path().filename() == ".myvc") continue;
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
    Tree t = getTreeAt(path).value();
    workingTree = t.hash();
    return t;
}

void RepositoryStore::applyOnWorkingTree(const TreeDiff &diff) {
    for(const auto &[path, change] : diff.getChanges()) {
        if(change.type == TreeChange::Type::Add || change.type == TreeChange::Type::Modify) {
            fs::create_directories(path.parent_path());
            std::ofstream out {path};
            change.newBlob.write(out);
        } else {
            fs::remove(path);
            for(fs::path par = path.parent_path(); par != "." && fs::is_directory(par) && fs::is_empty(par); par = par.parent_path()) {
                fs::remove(par);
            }
        }
    }
    workingTree = {};
}

void RepositoryStore::setWorkingTree(const Tree &tree) {
    TreeDiff diff = Tree::diff(getWorkingTree(), tree);
    applyOnWorkingTree(diff);
}

std::optional<Hash> RepositoryStore::resolvePartialObjectHash(const std::string &partial) {
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
    if(index) store(getIndexPath(), index.value());
    if(head) store(getHeadPath(), head.value());
    for(const auto &[h, optr] : objects) store(getObjectPath(h), *optr);
    for(const auto &[name, b] : branches) store(getBranchPath(name), b);
}
