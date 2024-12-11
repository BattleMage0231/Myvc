#pragma once

#include <iostream>
#include <string>
#include "hash.h"

namespace myvc {

class Serializable {
public:
    virtual void write(std::ostream &) const = 0;
    virtual void read(std::istream &) = 0;

    virtual ~Serializable() {}
};

template<typename T> void write_raw(std::ostream &out, const T &v) {
    const char *bytes = reinterpret_cast<const char *>(&v);
    out.write(bytes, sizeof(v));
}

template<typename T> void read_raw(std::istream &in, T &v) {
    char buf[sizeof(T)];
    in.read(buf, sizeof(T));
    v = *reinterpret_cast<T *>(buf);
}

void write_string(std::ostream &, const std::string &);
void read_string(std::istream &, std::string &);
void write_hash(std::ostream &, const Hash &);
void read_hash(std::istream &, Hash &);
void write_object(std::ostream &, const Serializable &);
void read_object(std::istream &, Serializable &);

}
