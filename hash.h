#pragma once

#include <compare>
#include <string>
#include <iostream>
#include "serialize.h"

namespace myvc {

class SHA1Hash : public Serializable {
    char bytes[20];

public:
    SHA1Hash();

    SHA1Hash(const Serializable &);

    SHA1Hash(const std::vector<char> &);

    SHA1Hash(const std::string &);

    std::strong_ordering operator<=>(const SHA1Hash &other) const;
    bool operator==(const SHA1Hash &) const;

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    operator std::string() const;
};

std::ostream &operator<<(std::ostream &, const SHA1Hash &);

using Hash = SHA1Hash;

}
