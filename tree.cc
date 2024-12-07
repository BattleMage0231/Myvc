#include "tree.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

TreeDiff Tree::diff(const Tree &a, const Tree &b) {
    return TreeDiff {a.getAllFiles(), b.getAllFiles()};
}

Tree::Node::Node(Hash dataHash, bool blob) : dataHash {std::move(dataHash)}, blob {blob} {}

void Tree::Node::write(std::ostream &out) const {
    write_raw(out, blob);
    write_hash(out, dataHash);
}

void Tree::Node::read(std::istream &in) {
    read_raw(in, blob);
    read_hash(in, dataHash);
}

std::variant<Tree, Blob> Tree::Node::getData() const {
    if(blob) return prov.lock()->getBlob(dataHash).value();
    else return prov.lock()->getTree(dataHash).value();
}

void Tree::Node::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
}

Tree::Tree(std::map<std::string, Node> nodes) : nodes {std::move(nodes)} {}

void Tree::write(std::ostream &out) const {
    write_raw(out, nodes.size());
    for(const auto &[name, node] : nodes) {
        write_string(out, name);
        write_object(out, node);
    }
}

void Tree::read(std::istream &in) {
    size_t sz;
    read_raw(in, sz);
    nodes.clear();
    for(size_t i = 0; i < sz; ++i) {
        std::string name;
        read_string(in, name);
        Node n;
        read_object(in, n);
        n.setProvider(prov);
        nodes.insert_or_assign(std::move(name), std::move(n));
    }
}

const std::map<std::string, Tree::Node> &Tree::getNodes() const {
    return nodes;
}

std::map<fs::path, Blob> Tree::getAllFiles() const {
    std::map<fs::path, Blob> files;
    for(const auto &[k, node] : nodes) {
        fs::path base = fs::path {k};
        auto data = node.getData();
        if(std::holds_alternative<Blob>(data)) {
            files.insert_or_assign(base, std::get<Blob>(data));
        } else {
            auto children = std::get<Tree>(data).getAllFiles();
            for(const auto &[p, blob] : children) {
                files.insert_or_assign(base / p, blob);
            }
        }
    }
    return files;
}

std::optional<std::variant<Tree, Blob>> Tree::getAtPath(const fs::path &path) const {
    std::string base = (*path.begin()).string();
    fs::path tail = fs::proximate(path, *path.begin());
    if(nodes.find(base) == nodes.end()) return {};
    Node n = nodes.at(base);
    auto data = n.getData();
    if(tail == ".") {
        return data;
    } else {
        if(std::holds_alternative<Blob>(data)) return {};
        else return std::get<Tree>(data).getAtPath(tail);
    }
}

void Tree::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
    for(auto &[k, v] : nodes) {
        v.setProvider(this->prov);
    }
}

Tree::Iterator Tree::begin() const {
    return nodes.begin();
}

Tree::Iterator Tree::end() const {
    return nodes.end();
}
