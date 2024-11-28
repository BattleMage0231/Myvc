#pragma once

#include "synced.h"
#include "hash.h"

namespace myvc {

class Object : public Synced {
public:
    void reload() override {}

    virtual Hash getHash() const = 0;
};

}
