#pragma once

#include "stored.h"
#include "hash.h"

namespace myvc {

class Object : public Stored {
public:
    void reload() override {}

    virtual Hash getHash() const = 0;
};

}
