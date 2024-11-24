module;

#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <map>
#include <memory>
#include <variant>
#include <optional>
#include <fstream>

export module repository_store;

export import commit;
export import branch;
export import hash;
export import head;

namespace myvc {

namespace fs = std::filesystem;

using std::invalid_argument, std::unique_ptr, std::map, std::variant, std::string, std::logic_error, std::optional, std::ifstream, std::ofstream;

export class RepositoryStore final {
    fs::path path;
    mutable map<Hash, Commit> commits;
    mutable map<string, Branch> branches;
    mutable optional<Head> head;

    template<typename T> T fetch(const fs::path &entityPath) const {
        ifstream in {entityPath};
        return T::read(in);
    }

    fs::path getMyvcPath() const {
        return path / ".myvc";
    }

    fs::path getObjectPath(const Hash &hash) const {
        return getMyvcPath() / "objects" / static_cast<string>(hash);
    }

    fs::path getBranchPath(const string &name) const {
        return getMyvcPath() / "refs" / "heads" / name;
    }

    fs::path getHeadPath() const {
        return getMyvcPath() / "HEAD";
    }

public:
    explicit RepositoryStore(const fs::path &path, bool create=false) {
        if(!fs::exists(path) || !fs::is_directory(path)) throw invalid_argument {"RepositoryStore::RepositoryStore() called with invalid path"};
        this->path = fs::canonical(path);
        if(!fs::exists(getMyvcPath())) {
            if(create) {
                fs::create_directories(getMyvcPath());
            } else {
                throw invalid_argument {"RepositoryStore::RepositoryStore() called with path that is not a repository and create=false"};
            }
        }
    }

    const Commit &getCommit(const Hash &hash) const {
        if(commits.find(hash) == commits.end()) {
            commits.insert_or_assign(hash, fetch<Commit>(getObjectPath(hash)));
        }
        return commits.at(hash);
    }

    Commit &createCommit(Commit commit) {
        Hash commitHash = commit.getHash();
        // todo check the values
        commits.insert_or_assign(commitHash, std::move(commit));
        return commits.at(commitHash);
    }

    Branch &getBranch(const string &name) {
        if(branches.find(name) == branches.end()) {
            branches.insert_or_assign(name, fetch<Branch>(getBranchPath(name)));
        }
        return branches.at(name);
    }

    const Branch &getBranch(const string &name) const {
        if(branches.find(name) == branches.end()) {
            branches.insert_or_assign(name, fetch<Branch>(getBranchPath(name)));
        }
        return branches.at(name);
    }

    Branch &createBranch(Branch branch) {
        const string &name = branch.getName();
        // todo check the values
        branches.insert_or_assign(name, std::move(branch));
        return branches.at(name);
    }

    const Head &getHead() const {
        if(!head) head = fetch<Head>(getHeadPath());
        return head.value();
    }

    Head &getHead() {
        if(!head) head = fetch<Head>(getHeadPath());
        return head.value();
    }

    void flush() {
        if(!commits.empty()) {
            if(!fs::exists(getMyvcPath() / "objects")) fs::create_directory(getMyvcPath() / "objects");
            for(const auto &pair : commits) {
                ofstream out {getObjectPath(pair.first)};
                std::cout << getObjectPath(pair.first) << std::endl;
                out << pair.second;
            }
        }
        if(!branches.empty()) {
            for(const auto &pair : branches) {
                if(!fs::exists(getMyvcPath() / "refs")) fs::create_directory(getMyvcPath() / "refs");
                ofstream out {getObjectPath(pair.first)};
                out << pair.second;
            }
        }
        if(head) {
            ofstream out {getHeadPath()};
            out << *head;
        }
    }

    ~RepositoryStore() {
        flush();
    }
};

}

