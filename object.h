#pragma once

#include <optional>
#include <sstream>
#include "serialize.h"

namespace myvc {

class Object : public Serializable {
    mutable std::optional<Hash> cachedHash;

public:
    Hash hash() const {
        if(cachedHash) return cachedHash.value();
        std::stringstream ss;
        write(ss);
        std::string str = ss.str();
        std::vector<char> data {str.begin(), str.end()};
        cachedHash = Hash { std::move(data) };
        return cachedHash.value();
    }

    bool operator==(const Object &other) const {
        return hash() == other.hash();
    }
};

}
