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

std::ostream &operator<<(std::ostream &out, const Serializable &s) {
    s.write(out);
    return out;
}

std::istream &operator>>(std::istream &in, Serializable &s) {
    s.read(in);
    return in;
}

template<typename T> void write_raw(std::ostream &out, const T &v) {
    const char *bytes = reinterpret_cast<const char *>(v);
    out.write(bytes, sizeof(v));
}

template<typename T> void read_raw(std::istream &in, T &v) {
    char buf[sizeof(T)];
    in.read(buf, sizeof(T));
    v = *reinterpret_cast<T *>(buf);
}

void write_string(std::ostream &out, const std::string &str) {
    write_raw(out, str.size());
    out.write(str.c_str(), str.size());
}

void read_string(std::istream &in, std::string &str) {
    size_t sz;
    read_raw(in, sz);
    char buf[sz];
    in.read(buf, sz * sizeof(char));
    str = std::string {buf, sz};
}

}
