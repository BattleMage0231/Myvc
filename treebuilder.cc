#include "treebuilder.h"

using namespace myvc;

TreeBuilder::TreeBuilder(std::weak_ptr<Provider> prov, Tree tree) : tree {std::move(tree)}, prov {std::move(prov)} {}

Tree TreeBuilder::updateEntryHelper(const Tree &t, const fs::path &path, Node node) {
    std::string base = (*path.begin()).string();
    fs::path tail = fs::proximate(path, *path.begin());
    std::map<std::string, Node> nodes = t.getNodes();
    if(tail == ".") {
        nodes.insert_or_assign(base, std::move(node));
    } else {
        Tree x;
        if(nodes.find(base) != nodes.end()) {
            x = std::get<Tree>(nodes.at(base).getData());
        }
        prov.lock()->createTree(x);
        Tree res = updateEntryHelper(x, tail, std::move(node));
        nodes.insert_or_assign(base, Node { res.hash(), false });
    }
    Tree res { std::move(nodes) };
    prov.lock()->createTree(res);
    return res;
}

void TreeBuilder::updateEntry(const fs::path &path, Node node) {
    auto data = node.getData();
    if(std::holds_alternative<Tree>(data) && std::get<Tree>(data).getNodes().empty()) {
        tree = deleteEntryHelper(tree, path);
    } else {
        tree = updateEntryHelper(tree, path, std::move(node));
    }
}

Tree TreeBuilder::deleteEntryHelper(const Tree &t, const fs::path &path) {
    std::string base = (*path.begin()).string();
    fs::path tail = fs::proximate(path, *path.begin());
    std::map<std::string, Node> nodes = t.getNodes();
    if(nodes.find(base) == nodes.end()) return t;
    if(tail == ".") {
        nodes.erase(base);
    } else {
        auto data = nodes.at(base).getData();
        if(std::holds_alternative<Tree>(data)) {
            Tree res = deleteEntryHelper(std::get<Tree>(data), tail);
            if(res.getNodes().empty()) {
                nodes.erase(base);
            } else {
                nodes.insert_or_assign(base, Node { res.hash(), false });
            }
        } else {
            return t;
        }
    }
    Tree res { std::move(nodes) };
    prov.lock()->createTree(res);
    return res;
}

bool TreeBuilder::deleteEntry(const fs::path &path) {
    Tree res = deleteEntryHelper(tree, path);
    if(res == tree) return false;
    tree = res;
    return true;
}

std::optional<std::variant<Tree, Blob>> TreeBuilder::getEntry(const fs::path &path) const {
    return tree.getAtPath(path);
}

void TreeBuilder::setTree(Tree t) {
    tree = std::move(t);
}

Tree TreeBuilder::getTree() const {
    return tree;
}
