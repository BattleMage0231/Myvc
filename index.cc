#include "errors.h"
#include "index.h"

using namespace myvc;

Index::Index(std::map<fs::path, Hash> blobs, std::shared_ptr<Provider> prov)
    : blobs {blobs}, prov {std::move(prov)} {}

Index::Index(std::istream &in, std::shared_ptr<Provider> prov) : prov {prov} {
    read(in);
}

void Index::write(std::ostream &out) const {
    write_raw(out, blobs.size());
    for(const auto &[k, v] : blobs) {
        write_string(out, static_cast<std::string>(k));
        out << v;
    }
}

void Index::read(std::istream &in) {
    size_t sz;
    read_raw(in, sz);
    for(size_t i = 0; i < sz; ++i) {
        std::string s;
        Hash h;
        read_string(in, s);
        in >> h;
        blobs[fs::path {s}] = h;
    }
}

void Index::reload() {
    *this = prov->getIndex();
}

void Index::store() {
    prov->updateIndex(*this);
}

void Index::addFile(fs::path path, Hash h) {
    blobs[std::move(path)] = h;
}

void Index::removeFile(const fs::path &path) {
    blobs.erase(path);
}

std::optional<Blob> Index::getFile(const fs::path &path) const {
    if(blobs.find(path) == blobs.end()) return {};
    else return prov->getBlob(blobs.at(path));
}

Tree Index::applyChanges(const Tree &tree) const {
    throw not_implemented {};
}

void Index::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
