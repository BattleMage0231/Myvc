#include "errors.h"
#include "index.h"
#include "treediff.h"

using namespace myvc;

Index::Index(Hash baseHash, Hash treeHash, std::shared_ptr<Provider> prov)
    : baseHash {baseHash}, treeHash {treeHash}, prov {std::move(prov)} {}

void Index::write(std::ostream &out) const {
    write_hash(out, baseHash);
    write_hash(out, treeHash);
}

void Index::read(std::istream &in) {
    read_hash(in, baseHash);
    read_hash(in, treeHash);
}

void Index::reload() {
    *this = prov->getIndex().value();
}

void Index::store() {
    prov->updateIndex(*this);
}

void Index::updateEntry(const fs::path &path, const Tree &tree) {
    Tree cur = getTree();
    cur.updateEntry(path, Tree::Node { tree.hash(), false });
    treeHash = cur.hash();
    store();
}

void Index::updateEntry(const fs::path &path, const Blob &blob) {
    Tree cur = getTree();
    cur.updateEntry(path, Tree::Node { blob.hash(), true });
    treeHash = cur.hash();
    store();
}

void Index::deleteEntry(const fs::path &path) {
    Tree tree = getTree();
    tree.deleteEntry(path);
    treeHash = tree.hash();
    store();
}

void Index::reset(Hash newBase) {
    baseHash = newBase;
    treeHash = newBase;
    store();
}

void Index::updateBase(Hash newBase) {
    baseHash = newBase;
    store();
}

void Index::updateTree(Hash newBase) {
    treeHash = newBase;
    store();
}

Tree Index::getBase() const {
    return prov->getTree(baseHash).value();
}

Tree Index::getTree() const {
    return prov->getTree(treeHash).value();
}

TreeDiff Index::getDiff() const {
    return Tree::diff(prov->getTree(baseHash).value(), getTree());
}

void Index::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
