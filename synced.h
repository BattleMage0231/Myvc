#pragma once

#include <iostream>
#include <vector>

namespace myvc {

class Synced {
public:
    virtual std::vector<char> serialize() const = 0;
    virtual void deserialize(std::vector<char>) = 0;
    virtual void reload() = 0;
    virtual void store() = 0;

    virtual ~Synced() {}

    friend std::istream &operator<<(std::istream &, const Synced &);
    friend std::ostream &operator>>(std::ostream &, Synced &);
};

}
