module;

#include <iostream>
#include <vector>
#include <type_traits>
#include <string>
#include <cstring>

export module serialization;

namespace myvc {

using std::string;

export using byte = unsigned char;
export using bytes = std::vector<byte>;

export std::ostream &operator<<(std::ostream &out, const bytes &b) {
    out.write((char *) b.data(), b.size());
    return out;
}

export template<typename T> bytes serialize(const T &v) requires std::is_integral_v<T>
{
    byte buffer[sizeof(T)];
    std::memcpy(buffer, &v, sizeof(T));
    return bytes {buffer, buffer + sizeof(T)};
}

export bytes serialize(const string &s) {
    bytes b {serialize(s.size())}, bs {s.begin(), s.end()};
    b.insert(b.end(), bs.begin(), bs.end());
    return b;
}

export template<typename T> bytes serialize(const std::vector<T> &v) {
    bytes b {serialize(v.size())};
    for(const auto &x : v) {
        bytes xs = serialize(x);
        b.insert(b.end(), xs.begin(), xs.end());
    }
    return b;
}

export std::istream &operator>>(std::istream &in, bytes &b) {
    // todo finish
    return in;
}

}
