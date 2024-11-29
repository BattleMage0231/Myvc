#include "tree.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

void Tree::Node::write(std::ostream &out) const {
    write_raw(out, isBlob);
    out << dataHash;
}

void Tree::Node::read(std::istream &in) {
    read_raw(in, isBlob);
    in >> dataHash;
}

Tree::Tree(std::map<std::string, Node> nodes, std::shared_ptr<Provider> prov)
    : nodes {std::move(nodes)}, prov {std::move(prov)} {}

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
        Node node;
        read_raw(in, node.isBlob);
        in >> node.dataHash;
        nodes.insert_or_assign(name, node);
    }
}

void Tree::store() {
    prov->createTree(*this);
}

Hash Tree::getHash() const {
    throw not_implemented {};
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

Tree::Iterator Tree::begin() {
    throw not_implemented {};
}

Tree::Iterator Tree::end() {
    throw not_implemented {};
}

Tree::ConstIterator Tree::begin() const {
    throw not_implemented {};
}

Tree::ConstIterator Tree::end() const {
    throw not_implemented {};
}
