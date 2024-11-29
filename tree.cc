#include "tree.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

Tree::Node::Node(Hash dataHash, bool blob, std::weak_ptr<Provider> prov)
    : dataHash {dataHash}, blob {blob}, prov {std::move(prov)} {}

Tree::Node::Node(std::istream &in, std::weak_ptr<Provider> prov) : prov {prov} {
    read(in);
}

void Tree::Node::write(std::ostream &out) const {
    write_raw(out, blob);
    out << dataHash;
}

void Tree::Node::read(std::istream &in) {
    read_raw(in, blob);
    in >> dataHash;
}

bool Tree::Node::isBlob() const {
    return blob;
}

std::variant<Tree, Blob> Tree::Node::getData() const {
    auto p = prov.lock();
    if(isBlob()) return p->getBlob(dataHash);
    else return p->getTree(dataHash);
}

std::variant<Tree, Blob> Tree::Node::operator*() const {
    return getData();
}

void Tree::Node::setBlob(Hash hash) {
    blob = true;
    dataHash = hash;
}

void Tree::Node::setTree(Hash hash) {
    blob = false;
    dataHash = hash;
}

void Tree::Node::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
}

Tree::Tree(std::map<std::string, Node> nodes, std::shared_ptr<Provider> prov)
    : nodes {std::move(nodes)}, prov {std::move(prov)}
{
    for(auto &[k, v] : nodes) {
        v.setProvider(this->prov);
    }
}

Tree::Tree(std::istream &in, std::shared_ptr<Provider> prov) : prov {prov} {
    read(in);
}

void Tree::write(std::ostream &out) const {
    write_raw(out, nodes.size());
    for(const auto &[name, node] : nodes) {
        write_string(out, name);
        out << node;
    }
}

void Tree::read(std::istream &in) {
    size_t sz;
    read_raw(in, sz);
    nodes.clear();
    for(size_t i = 0; i < sz; ++i) {
        std::string name;
        read_string(in, name);
        bool blob;
        read_raw(in, blob);
        Hash h;
        in >> h;
        nodes.insert_or_assign(std::move(name), Node {h, blob, prov});
    }
}

void Tree::store() {
    prov->createTree(*this);
}

std::map<std::string, Tree::Node> &Tree::getNodes() {
    return nodes;
}

const std::map<std::string, Tree::Node> &Tree::getNodes() const {
    return nodes;
}

void Tree::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
}

Tree::Iterator Tree::begin() const {
    return nodes.begin();
}

Tree::Iterator Tree::end() const {
    return nodes.end();
}
