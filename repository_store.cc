module;

#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <set>
#include <optional>
#include <fstream>
#include <utility>
#include <vector>
#include <functional>

export module repository_store;

import object;
import tree;
import commit;
import branch;
import hash;
import head;

namespace myvc {

namespace fs = std::filesystem;

using std::set, std::string, std::vector;

export class RepositoryStore final {
    fs::path path;
    set<Commit> commits;
    set<Tree> trees;
    vector<Branch> branches;
    std::optional<Head> head;

    template<typename T> T fetch(const fs::path &entityPath) {
        std::ifstream in {entityPath};
        return T::read(in);
    }

    template<typename T> void store(const T &data, const fs::path &path) {
        std::ofstream out {path};
        out << data;
    }

    fs::path getMyvcPath() const {
        return path / ".myvc";
    }

    fs::path getObjectPathFromHash(const Hash &h) const {
        return getMyvcPath() / "objects" / static_cast<string>(h);
    }

    fs::path getObjectPath(const Object &o) const {
        return getObjectPathFromHash(o.getHash());
    }

    fs::path getBranchPathFromName(const string &name) const {
        return getMyvcPath() / "refs" / "heads" / name;
    }

    fs::path getBranchPath(const Branch &b) const {
        return getBranchPathFromName(b.getName());
    }

    fs::path getHeadPath() const {
        return getMyvcPath() / "HEAD";
    }

    std::optional<std::reference_wrapper<Branch>> getCachedBranch(const string &name) {
        for(auto &b : branches) {
            if(b.getName() == name) return b;
        }
        return {};
    }

public:
    explicit RepositoryStore(const fs::path &path, bool create=false) {
        if(!fs::exists(path) || !fs::is_directory(path)) throw std::invalid_argument {"RepositoryStore::RepositoryStore() called with invalid path"};
        this->path = fs::canonical(path);
        if(!fs::exists(getMyvcPath())) {
            if(create) {
                fs::create_directories(getMyvcPath());
            } else {
                throw std::invalid_argument {"RepositoryStore::RepositoryStore() called with path that is not a repository and create=false"};
            }
        }
    }

    const Commit &getCommit(const Hash &hash) {
        for(const auto &c : commits) {
            if(c.getHash() == hash) return c; 
        }
        return createCommit(fetch<Commit>(getObjectPathFromHash(hash)));
    }

    const Commit &createCommit(Commit commit) {
        auto res = commits.insert(std::move(commit));
        return *res.first;
    }

    const Tree &getTree(const Hash &hash) {
        for(const auto &c : trees) {
            if(c.getHash() == hash) return c; 
        }
        return createTree(fetch<Tree>(getObjectPathFromHash(hash)));
    }

    const Tree &createTree(Tree tree) {
        auto res = trees.insert(std::move(tree));
        return *res.first;
    }

    Branch &getBranch(const string &name) {
        auto cached = getCachedBranch(name);
        if(cached) return cached.value();
        else return createBranch(fetch<Branch>(getBranchPathFromName(name)));
    }

    Branch &createBranch(Branch branch) {
        auto cached = getCachedBranch(branch.getName());
        if(cached) {
            throw std::invalid_argument {"Branch::createBranch() called on branch with existing name"};
        } else {
            branches.push_back(std::move(branch));
            return branches.back();
        }
    }

    Head &getHead() {
        if(!head) head = fetch<Head>(getHeadPath());
        return head.value();
    }

    void flush() {
        if(!commits.empty() || !trees.empty()) {
            if(!fs::exists(getMyvcPath() / "objects")) fs::create_directory(getMyvcPath() / "objects");
            for(const auto &v : commits) store(v, getObjectPath(v));
            for(const auto &v : trees) store(v, getObjectPath(v));
        }
        if(!branches.empty()) {
            if(!fs::exists(getMyvcPath() / "refs")) fs::create_directory(getMyvcPath() / "refs");
            for(const auto &v : branches) store(v, getBranchPath(v));
        }
        if(head) store(head.value(), getHeadPath());
    }

    ~RepositoryStore() {
        try {
            flush();
        } catch(...) {}
    }
};

}
