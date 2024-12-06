#pragma once

#include <optional>
#include "stored.h"
#include "hash.h"

namespace myvc {

class Object : public Stored {
    mutable std::optional<Hash> cachedHash;

public:
    void reload() override {}

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
