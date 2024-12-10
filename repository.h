#pragma once

#include <vector>
#include <optional>
#include "store.h"

namespace myvc {

class Repository : public RepositoryStore {
    std::optional<std::vector<fs::path>> threeWayMerge(const Tree &, const Tree &, const Tree &);

public:
    static constexpr std::string defaultBranch = "main";

    explicit Repository(fs::path);

    void addToIndex(const std::vector<fs::path> &);
    void removeFromIndex(const std::vector<fs::path> &, bool);
    void commitIndex(std::string, std::set<Hash> otherParents = {});
    void moveHeadSticky(const Hash &);
    void checkout(const std::string &);
    std::optional<std::vector<fs::path>> merge(const Hash &);
    std::optional<std::vector<fs::path>> cherrypick(const Hash &);
};

}
