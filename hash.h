#pragma once

#include <compare>
#include <string>
#include <iostream>
#include <optional>
#include <vector>

namespace myvc {

class SHA1Hash {
    char bytes[20];
    mutable std::optional<std::string> cachedHex;

public:
    SHA1Hash();

    explicit SHA1Hash(const std::vector<char> &);

    explicit SHA1Hash(const std::string &);

    std::strong_ordering operator<=>(const SHA1Hash &other) const;
    bool operator==(const SHA1Hash &) const;

    operator std::string() const;
};

std::ostream &operator<<(std::ostream &, const SHA1Hash &);

using Hash = SHA1Hash;

}
