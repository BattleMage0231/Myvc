export module repository_store;

export import <filesystem>;
export import <stdexcept>;
import <vector>;
import <map>;
import <memory>;
import <variant>;

import commit;
import branch;
import hash;

namespace myvc {

namespace fs = std::filesystem;

using std::invalid_argument, std::vector, std::unique_ptr, std::shared_ptr, std::map, std::reference_wrapper, std::variant, std::string;

using Head = std::variant<reference_wrapper<Branch>, const reference_wrapper<Commit>>;
using ConstHead = std::variant<const reference_wrapper<Branch>, const reference_wrapper<Commit>>;

export class RepositoryStore final {
    fs::path path;
    vector<unique_ptr<Writable>> entities;
    map<Hash, size_t> commits;
    map<string, size_t> branches;

public:
    explicit RepositoryStore(const fs::path &path, bool create=false) : path {path} {
        // todo validate
        // todo implement create
    }

    fs::path getMyvcPath() const {
        return path / ".myvc";
    }

    const Commit &getCommit(const Hash &hash) {
        const auto &entity = entities.at(commits.at(hash));
        return dynamic_cast<Commit &>(*entity);
    }

    void createCommit(unique_ptr<Commit> commit) {
        const Hash &commitHash = commit.getHash();
        const Hash &treeHash = commit.getTreeHash();
        // todo test for tree
        const vector<Hash> &parentHashes = commit.getParentHashes();
        for(const auto &commitHash : parentHashes) {
            if(commits.find(treeHash) == commits.end()) throw logic_error {"RepositoryStore::createCommit() called with a commit with an invalid parent hash"};
        }
        entites.push_back(std::move(commit));
        commits.insert({commitHash, entities.size() - 1});
    }

    Branch &getBranch(const string &name) {
        const auto &entity = entities.at(branches.at(name));
        return dynamic_cast<Branch &>(*entity);
    }

    void createBranch(unique_ptr<Branch> branch) {
        const string &name = branch->getName();
        const Hash &commit = *branch;
        if(commits.find(commit) == commits.end()) throw logic_error {"RepositoryStore::createBranch() called with a branch with an invalid commit"};
        entities.push_back(std::move(branch));
        commits.insert({name, entities.size() - 1});
    }

    /*
    ConstHead getHead() const {

    }

    Head getHead() {

    }
    */

    void moveHead() {

    }

    void moveHead() {

    }

    void flush() {
        const fs::path &myvcPath = getMyvcPath();
        for(const auto& e : entites) e.writeWithBase(myvcPath); 
    }

    ~RepositoryStore() {
        flush();
    }
};

}

