#include "errors.h"
#include "index.h"
#include "treediff.h"

using namespace myvc;

Index::Index(Hash baseHash, Hash treeHash, std::weak_ptr<Provider> prov)
    : baseHash {std::move(baseHash)}, treeHash {std::move(treeHash)}, prov {std::move(prov)} {}

void Index::write(std::ostream &out) const {
    write_hash(out, baseHash);
    write_hash(out, treeHash);
}

void Index::read(std::istream &in) {
    read_hash(in, baseHash);
    read_hash(in, treeHash);
}

/*
void Index::updateEntry(const fs::path &path, const Tree &tree) {
    Tree cur = getTree();
    cur.updateEntry(path, Tree::Node { tree.hash(), false });
    treeHash = cur.hash();
}

void Index::updateEntry(const fs::path &path, const Blob &blob) {
    Tree cur = getTree();
    cur.updateEntry(path, Tree::Node { blob.hash(), true });
    treeHash = cur.hash();
}

void Index::deleteEntry(const fs::path &path) {
    Tree tree = getTree();
    tree.deleteEntry(path);
    treeHash = tree.hash();
}

void Index::reset(Hash newBase) {
    baseHash = newBase;
    treeHash = newBase;
}

void Index::updateBase(Hash newBase) {
    baseHash = newBase;
}

void Index::updateTree(Hash newBase) {
    treeHash = newBase;
}
*/

Tree Index::getBase() const {
    return prov.lock()->getTree(baseHash).value();
}

Tree Index::getTree() const {
    return prov.lock()->getTree(treeHash).value();
}

TreeDiff Index::getDiff() const {
    return Tree::diff(getBase(), getTree());
}

void Index::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
