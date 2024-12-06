#pragma once

#include <optional>
#include "serialize.h"

namespace myvc {

class Object : public Serializable {
    mutable std::optional<Hash> cachedHash;

public:
    Hash hash() const override {
        if(cachedHash) return cachedHash.value();
        cachedHash = Serializable::hash();
        return cachedHash.value();
    }

    bool operator==(const Object &other) const {
        return hash() == other.hash();
    }
};

}
