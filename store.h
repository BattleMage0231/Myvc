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
#include "treebuilder.h"

namespace myvc {

namespace fs = std::filesystem;

class RepositoryStore :
    public Commit::Provider,
    public Branch::Provider,
    public Head::Provider,
    public Index::Provider,
    public TreeBuilder::Provider
{
    fs::path path;

    mutable std::map<Hash, std::unique_ptr<Object>> objects;
    mutable std::map<std::string, Branch> branches;
    mutable std::optional<Index> index;
    mutable std::optional<Head> head;
    mutable std::optional<Hash> workingTree;
    mutable std::shared_ptr<RepositoryStore> self;

    std::shared_ptr<RepositoryStore> getInstance() const;

    fs::path getMyvcPath() const;
    fs::path getObjectPath(const Hash &) const;
    fs::path getBranchPath(const std::string &) const;
    fs::path getHeadPath() const;
    fs::path getIndexPath() const;

    template<typename T> std::optional<T> load(const fs::path &) const;
    void store(const fs::path &, const Serializable &);

    template<typename T> bool createObject(T &);
    template<typename T> std::optional<T> loadObject(const Hash &) const;

public:
    static constexpr std::string myvcName = ".myvc";

    static bool existsAt(const fs::path &);
    static bool createAt(const fs::path &);

    explicit RepositoryStore(fs::path);

    bool createCommit(Commit &);
    bool createTree(Tree &);
    bool createBlob(Blob &);
    bool createBranch(std::string, Hash);

    const fs::path &getPath() const;
    std::optional<Commit> getCommit(const Hash &) const;
    std::optional<Tree> getTree(const Hash &) const;
    std::optional<Blob> getBlob(const Hash &) const;
    std::optional<std::reference_wrapper<Branch>> getBranch(const std::string &);
    std::optional<const std::reference_wrapper<Branch>> getBranch(const std::string &) const;
    Index &getIndex();
    Head &getHead();

    bool deleteBranch(const Branch &);

    TreeBuilder makeTreeBuilder(Tree);

    std::optional<Blob> getBlobAt(const fs::path &);
    std::optional<Tree> getTreeAt(const fs::path &);
    Tree getWorkingTree();
    void applyOnWorkingTree(const TreeDiff &);
    void setWorkingTree(const Tree &);

    std::optional<Hash> resolvePartialHash(const std::string &);

    virtual ~RepositoryStore();
};

}
