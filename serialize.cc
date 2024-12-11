#include <sstream>
#include "serialize.h"

using namespace myvc;

void myvc::write_string(std::ostream &out, const std::string &str) {
    write_raw(out, str.size());
    out.write(str.c_str(), str.size());
}

void myvc::read_string(std::istream &in, std::string &str) {
    size_t sz;
    read_raw(in, sz);
    char buf[sz];
    in.read(buf, sz * sizeof(char));
    str = std::string {buf, sz};
}

void myvc::write_hash(std::ostream &out, const Hash &hash) {
    write_string(out, static_cast<std::string>(hash));
}

void myvc::read_hash(std::istream &in, Hash &hash) {
    std::string hex;
    read_string(in, hex);
    hash = Hash {hex};
}

void myvc::write_object(std::ostream &out, const Serializable &o) {
    o.write(out);
}

void myvc::read_object(std::istream &in, Serializable &o) {
    o.read(in);
}
