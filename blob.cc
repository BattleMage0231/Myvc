#include "blob.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

Blob::Blob(std::vector<char> data, std::shared_ptr<Provider> prov) 
    : data {std::move(data)}, prov {std::move(prov)} {}


Blob::Blob(std::istream &in, std::shared_ptr<Provider> prov) : prov {std::move(prov)} {
    read(in);
}

void Blob::write(std::ostream &out) const {
    write_raw(out, data.size());
    for(char c : data) write_raw(out, c);
}

void Blob::read(std::istream &in) {
    size_t sz;
    read_raw(in, sz);
    data.clear();
    data.reserve(sz);
    for(size_t i = 0; i < sz; ++i) {
        char c;
        read_raw(in, c);
        data.push_back(c);
    }
}

void Blob::store() {
    prov->createBlob(*this);
}

std::vector<char> &Blob::getData() {
    return data;
}

const std::vector<char> &Blob::getData() const {
    return data;
}

void Blob::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
}

