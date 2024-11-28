#pragma once

#include <string>

namespace myvc {

struct Hash {
    std::string s;

    auto operator<=>(const Hash &) const = default;

    bool operator==(const Hash &) const = default;
};

}
