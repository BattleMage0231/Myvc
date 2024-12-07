#include "errors.h"
#include "index.h"

using namespace myvc;

Index::Index(Hash treeHash) : treeHash {std::move(treeHash)} {}

void Index::write(std::ostream &out) const {
    write_hash(out, treeHash);
}

void Index::read(std::istream &in) {
    read_hash(in, treeHash);
}

Tree Index::getTree() const {
    if(!prov.lock()) THROW("nonexistent provider");
    return prov.lock()->getTree(treeHash).value();
}

void Index::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
