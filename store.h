#pragma once

#include <filesystem>
#include <optional>
#include <functional>
#include <memory>
#include "object.h"
#include "tree.h"
#include "commit.h"
#include "branch.h"
#include "hash.h"
#include "head.h"
#include "blob.h"
#include "index.h"
#include "treediff.h"

namespace myvc {

namespace fs = std::filesystem;

class RepositoryStore :
    public Commit::Provider,
    public Tree::Provider,
    public Branch::Provider,
    public Head::Provider,
    public Index::Provider,
    public std::enable_shared_from_this<RepositoryStore>
{
    fs::path path;

    mutable std::map<Hash, std::unique_ptr<Object>> objects;
    mutable std::map<std::string, Branch> branches;
    mutable std::optional<Index> index;
    mutable std::optional<Head> head;
    mutable std::optional<Hash> workingTree;

    std::weak_ptr<RepositoryStore> getInstance() const;

    fs::path getMyvcPath() const;
    fs::path getObjectPath(const Hash &) const;
    fs::path getBranchPath(const std::string &) const;
    fs::path getHeadPath() const;
    fs::path getIndexPath() const;

    template<typename T> std::optional<T> load(const fs::path &) const;
    void store(const fs::path &, const Serializable &);

    template<typename T> bool createObject(const T &);
    template<typename T> std::optional<T> loadObject(const Hash &) const;

public:
    static constexpr std::string myvcName = ".myvc";

    static bool createAt(const fs::path &);

    explicit RepositoryStore(fs::path);

    bool createCommit(const Commit &);
    bool createTree(const Tree &);
    bool createBlob(const Blob &);
    bool createBranch(std::string, Hash);

    std::optional<Commit> getCommit(const Hash &) const;
    std::optional<Tree> getTree(const Hash &) const;
    std::optional<Blob> getBlob(const Hash &) const;
    std::optional<std::reference_wrapper<Branch>> getBranch(const std::string &);
    std::optional<const std::reference_wrapper<Branch>> getBranch(const std::string &) const;
    Index &getIndex();
    Head &getHead();

    bool deleteBranch(const Branch &);

    std::optional<Blob> getBlobAt(const fs::path &);
    std::optional<Tree> getTreeAt(const fs::path &);
    Tree getWorkingTree();
    void applyOnWorkingTree(const TreeDiff &);
    void setWorkingTree(const Tree &);

    std::optional<Hash> resolvePartialObjectHash(const std::string &);

    ~RepositoryStore();
};

}
