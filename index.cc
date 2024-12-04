#include "errors.h"
#include "index.h"
#include "treediff.h"

using namespace myvc;

Index::Index(Hash baseHash, Hash treeHash, std::shared_ptr<Provider> prov)
    : baseHash {baseHash}, treeHash {treeHash}, prov {std::move(prov)} {}

Index::Index(std::istream &in, std::shared_ptr<Provider> prov) : prov {prov} {
    read(in);
}

void Index::write(std::ostream &out) const {
    baseHash.write(out);
    treeHash.write(out);
}

void Index::read(std::istream &in) {
    baseHash.read(in);
    treeHash.read(in);
}

void Index::reload() {
    *this = prov->getIndex().value();
}

void Index::store() {
    prov->updateIndex(*this);
}

void Index::updateEntry(const fs::path &path, const Tree &tree) {
    Tree cur = getTree();
    cur.updateEntry(path, Tree::Node { tree.getHash(), false });
    treeHash = cur.getHash();
    store();
}

void Index::updateEntry(const fs::path &path, const Blob &blob) {
    Tree cur = getTree();
    cur.updateEntry(path, Tree::Node { blob.getHash(), true });
    treeHash = cur.getHash();
    store();
}

void Index::deleteEntry(const fs::path &path) {
    Tree tree = getTree();
    tree.deleteEntry(path);
    treeHash = tree.getHash();
    store();
}

void Index::reset(Hash newBase) {
    baseHash = newBase;
    treeHash = newBase;
    store();
}

void Index::updateBase(Hash newBase) {
    Tree newBaseTree = prov->getTree(newBase).value();
    TreeDiff diff = getDiff();
    if(!diff.getChanges().empty()) {
        throw not_implemented {};
    }
    baseHash = newBaseTree.getHash();
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
