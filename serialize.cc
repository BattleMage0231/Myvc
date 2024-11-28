#include "serialize.h"

using namespace myvc;

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
