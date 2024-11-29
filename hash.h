#pragma once

#include <string>
#include "serialize.h"

namespace myvc {

class SHA1Hash : public Serializable {
    char bytes[20];

public:
    SHA1Hash();

    SHA1Hash(const Serializable &s);

    auto operator<=>(const SHA1Hash &) const = default;

    bool operator==(const SHA1Hash &) const = default;

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    operator std::string() const;
};

using Hash = SHA1Hash;

}
