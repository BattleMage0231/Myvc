#include "tree.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

TreeDiff Tree::diff(const Tree &a, const Tree &b) {
    return TreeDiff {a.getAllFiles(), b.getAllFiles()};
}

Tree::Node::Node(Hash dataHash, bool blob, std::weak_ptr<Provider> prov)
    : dataHash {dataHash}, blob {blob}, prov {std::move(prov)} {}

Tree::Node::Node(std::istream &in, std::weak_ptr<Provider> prov) : prov {prov} {
    read(in);
}

void Tree::Node::write(std::ostream &out) const {
    write_raw(out, blob);
    write_hash(out, dataHash);
}

void Tree::Node::read(std::istream &in) {
    read_raw(in, blob);
    read_hash(in, dataHash);
}

bool Tree::Node::isBlob() const {
    return blob;
}

std::variant<Tree, Blob> Tree::Node::getData() const {
    auto p = prov.lock();
    if(isBlob()) return p->getBlob(dataHash).value();
    else return p->getTree(dataHash).value();
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
    for(auto &[k, v] : this->nodes) {
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
        node.write(out);
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
        n.read(in);
        n.setProvider(prov);
        nodes.insert_or_assign(std::move(name), std::move(n));
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

std::map<fs::path, Blob> Tree::getAllFiles() const {
    std::map<fs::path, Blob> files;
    for(const auto &[k, node] : nodes) {
        fs::path base = fs::path {k};
        if(node.isBlob()) {
            files.insert_or_assign(base, std::get<Blob>(node.getData()));
        } else {
            std::map<fs::path, Blob> children = std::get<Tree>(node.getData()).getAllFiles();
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
    if(tail == ".") {
        return *n;
    } else {
        if(n.isBlob()) return {};
        else return std::get<Tree>(n.getData()).getAtPath(tail);
    }
}

void Tree::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
    for(auto &[k, v] : nodes) {
        v.setProvider(this->prov);
    }
}

void Tree::updateEntry(const fs::path &path, Node node) {
    std::string base = (*path.begin()).string();
    fs::path tail = fs::proximate(path, *path.begin());
    node.setProvider(prov);
    if(tail == ".") {
        nodes.insert_or_assign(base, std::move(node));
    } else {
        Tree t {{}, prov};
        if(nodes.find(base) != nodes.end()) {
            t = std::get<Tree>(nodes[base].getData());
        }
        t.updateEntry(tail, std::move(node));
        nodes[base].setTree(t.hash());
    }
    store();
}

void Tree::deleteEntry(const fs::path &path) {
    std::string base = (*path.begin()).string();
    fs::path tail = fs::proximate(path, *path.begin());
    if(nodes.find(base) == nodes.end()) return;
    if(tail == ".") {
        nodes.erase(base);
    } else {
        Tree t = std::get<Tree>(nodes[base].getData());
        t.deleteEntry(tail);
        if(t.nodes.empty()) {
            nodes.erase(base);
        } else {
            nodes[base].setTree(t.hash());
        }
    }
    store();
}

Tree::Iterator Tree::begin() const {
    return nodes.begin();
}

Tree::Iterator Tree::end() const {
    return nodes.end();
}
