#pragma once

#include <vector>
#include <optional>
#include "store.h"

namespace myvc {

class Repository {
    fs::path path;
    mutable RepositoryStore store;

public:
    explicit Repository(fs::path);

    Head &getHead() const;
    Index &getIndex() const;
    Tree getWorkingTree() const;
    std::optional<std::reference_wrapper<Branch>> getBranch(const std::string &);
    std::optional<Commit> getCommit(const Hash &) const;
    std::optional<Hash> resolvePartialHash(std::string) const;

    void addToIndex(const std::vector<fs::path> &);
};

}
