#pragma once

#include <string>
#include "serialize.h"

namespace myvc {

struct Hash : public Serializable {
    std::string s;

    auto operator<=>(const Hash &) const = default;

    bool operator==(const Hash &) const = default;

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    operator std::string() const {
        return s;
    }
};

}
