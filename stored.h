#pragma once

#include "serialize.h"

namespace myvc {

class Stored : public Serializable {
public:
    virtual void reload() = 0;
    virtual void store() = 0;
};

}
