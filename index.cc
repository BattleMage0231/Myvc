#include "errors.h"
#include "index.h"

using namespace myvc;

Index::Index(Hash treeHash, std::weak_ptr<Provider> prov)
    : treeHash {std::move(treeHash)}, prov {std::move(prov)} {}

void Index::write(std::ostream &out) const {
    write_hash(out, treeHash);
}

void Index::read(std::istream &in) {
    read_hash(in, treeHash);
}

Tree Index::getTree() const {
    return prov.lock()->getTree(treeHash).value();
}

void Index::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
