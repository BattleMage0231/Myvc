module;

#include <set>
#include <string>
#include <stdexcept>
#include <compare>

export module tree;

import writable;
import hash;
import object;

namespace myvc {

using std::string, std::set;

export class TreeNode : public Writable {
    string id;
    bool blob;
    Hash hash;

    void write(std::ostream &out) const noexcept override {
        out << id << '\n' << (isBlob() ? "Blob" : "Dir") << '\n' << hash << '\n';
    }

public:
    TreeNode(string id, bool blob, Hash hash) : id {std::move(id)}, blob {blob}, hash {hash} {}

    static TreeNode read(std::istream &in) {
        string id, b; Hash hash;
        in >> id >> b >> hash;
        return TreeNode(id, b == "Blob", hash);
    }

    const string &getId() const {
        return id;
    }

    bool isBlob() const {
        return blob;
    }

    const Hash &getHash() const {
        return hash;
    }

    auto operator<=>(const TreeNode &other) const noexcept {
        if(blob != other.blob) return blob <=> other.blob;
        if(id != other.id) return id <=> other.id;
        return hash <=> other.hash;
    }
};

export class Tree : public Object {
    set<TreeNode> nodes;

    void write(std::ostream &out) const noexcept override {
        out << "Tree" << '\n';
        out << nodes.size() << '\n';
        for(const auto &node : nodes) out << node;
    }

public:
    using Iterator = set<TreeNode>::const_iterator;

    explicit Tree(set<TreeNode> nodes = {}) : nodes {std::move(nodes)} {}
    
    static Tree read(std::istream &in) {
        string s; set<TreeNode> nodes; size_t sz;
        in >> s;
        if(s == "Tree") {
            in >> sz;
            for(size_t i = 0; i < sz; ++i) {
                nodes.insert(TreeNode::read(in));
            }
            return Tree {nodes};
        }
        throw std::invalid_argument {"Tree::read() failed parsing"};
    }

    Hash getHash() const noexcept override {
        // todo implement this
        return Hash {"temporary_hash"};
    }

    Iterator begin() const {
        return nodes.begin();
    }

    Iterator end() const {
        return nodes.end();
    }
};

}
