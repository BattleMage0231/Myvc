#pragma once

#include <filesystem>
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

    template<typename T> T load(const fs::path &) const;
    void store(const fs::path &, const Serializable &); 

    Tree getTreeAt(const fs::path &);
    void setTreeAt(const fs::path &, const Tree &);

public:
    explicit RepositoryStore(fs::path);

    Commit getCommit(Hash) const override;
    void createCommit(const Commit &) override;
    Tree getTree(Hash) const override;
    void createTree(const Tree &) override;
    Blob getBlob(Hash) const override;
    void createBlob(const Blob &) override;

    Branch getBranch(const std::string &) const override;
    void deleteBranch(const std::string &);
    void updateBranch(const Branch &) override;

    Head getHead() const override;
    void updateHead(const Head &) override;
    Index getIndex() const override;
    void updateIndex(const Index &) override;

    Tree getWorkingTree();
    void setWorkingTree(const Tree &);

    Hash resolvePartialObjectHash(const std::string &);

    static void createAt(const fs::path &);
};

}
