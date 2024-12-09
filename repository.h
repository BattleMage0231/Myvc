#pragma once

#include <vector>
#include <optional>
#include "store.h"

namespace myvc {

class Repository : public RepositoryStore {
public:
    static constexpr std::string defaultBranch = "main";

    explicit Repository(fs::path);

    void addToIndex(const std::vector<fs::path> &);
    void removeFromIndex(const std::vector<fs::path> &, bool);
    void commitIndex(std::string, std::set<Hash> otherParents = {});
    void moveHeadSticky(const Hash &);
    void checkout(const std::string &);
};

}
