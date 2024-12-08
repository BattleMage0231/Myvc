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
    void commitIndex(std::string, std::set<Hash> otherParents = {});
};

}
