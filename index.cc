#include "errors.h"
#include "index.h"

using namespace myvc;

Index::Index(Hash baseHash, Hash treeHash, std::shared_ptr<Provider> prov)
    : baseHash {baseHash}, treeHash {treeHash}, prov {std::move(prov)} {}

Index::Index(std::istream &in, std::shared_ptr<Provider> prov) : prov {prov} {
    read(in);
}

void Index::write(std::ostream &out) const {
    out << baseHash << treeHash;
}

void Index::read(std::istream &in) {
    in >> baseHash >> treeHash;
}

void Index::reload() {
    *this = prov->getIndex().value();
}

void Index::store() {
    prov->updateIndex(*this);
}

void Index::updateEntry(const fs::path &path, const Tree &tree) {
    throw not_implemented {};
}

void Index::updateEntry(const fs::path &path, const Blob &blob) {
    throw not_implemented {};
}

void Index::deleteEntry(const fs::path &path) {
    throw not_implemented {};
}

void Index::updateBase(Hash newBase) {
    throw not_implemented {};
}

Tree Index::getTree() const {
    return prov->getTree(treeHash).value();
}

void Index::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
