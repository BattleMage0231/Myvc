#pragma once

#include <filesystem>
#include <optional>
#include "object.h"
#include "tree.h"
#include "commit.h"
#include "branch.h"
#include "hash.h"
#include "head.h"
#include "blob.h"
#include "index.h"

namespace myvc {

namespace fs = std::filesystem;

class RepositoryStore :
    public Commit::Provider,
    public Tree::Provider,
    public Blob::Provider,
    public Branch::Provider,
    public Head::Provider,
    public Index::Provider
{
    fs::path path;

    fs::path getMyvcPath() const;
    fs::path getObjectPath(Hash) const;
    fs::path getBranchPath(const std::string &) const;
    fs::path getHeadPath() const;
    fs::path getIndexPath() const;

    std::shared_ptr<RepositoryStore> getInstance() const;

    template<typename T> std::optional<T> load(const fs::path &) const;
    void store(const fs::path &, const Serializable &); 

public:
    explicit RepositoryStore(fs::path);

    std::optional<Commit> getCommit(Hash) const override;
    void createCommit(const Commit &) override;
    std::optional<Tree> getTree(Hash) const override;
    void createTree(const Tree &) override;
    std::optional<Blob> getBlob(Hash) const override;
    void createBlob(const Blob &) override;

    std::optional<Branch> getBranch(const std::string &) const override;
    void deleteBranch(const std::string &);
    void updateBranch(const Branch &) override;

    std::optional<Head> getHead() const override;
    void updateHead(const Head &) override;
    std::optional<Index> getIndex() const override;
    void updateIndex(const Index &) override;

    Tree getTreeAt(const fs::path &);
    Blob getBlobAt(const fs::path &);
    Tree getWorkingTree();
    void setWorkingTree(const Tree &);

    std::optional<Hash> resolvePartialObjectHash(const std::string &);

    static bool createAt(const fs::path &);
};

}
