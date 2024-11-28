#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>

namespace myvc {

class Serializable {
public:
    virtual void write(std::ostream &) const = 0;
    virtual void read(std::istream &) = 0;

    virtual ~Serializable() {}
};

std::ostream &operator<<(std::ostream &, const Serializable &);
std::istream &operator>>(std::istream &, Serializable &);

template<typename T> void write_raw(std::ostream &, const T &);
template<typename T> void read_raw(std::istream &, T &);

}
