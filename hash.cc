#include "hash.h"
#include "serialize.h"

using namespace myvc;

void Hash::write(std::ostream &out) const {
    write_string(out, s);
}

void Hash::read(std::istream &in) {
    read_string(in, s);
}

Hash::operator std::string() const {
    return s;
}
