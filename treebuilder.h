#pragma once

#include <filesystem>
#include "tree.h"

namespace myvc {

namespace fs = std::filesystem;

class TreeBuilder {
public:
    class Provider : public Tree::Provider {
    public:
        virtual bool createTree(Tree &) = 0;
    };

private:
    using Node = Tree::Node;

    Tree tree;
    std::weak_ptr<Provider> prov;

    Tree updateEntryHelper(const Tree &, const fs::path &, Node);
    Tree deleteEntryHelper(const Tree &, const fs::path &);

public:
    explicit TreeBuilder(std::weak_ptr<Provider>, Tree tree = Tree {});

    void updateEntry(const fs::path &, Node);
    bool deleteEntry(const fs::path &);
    std::optional<std::variant<Tree, Blob>> getEntry(const fs::path &) const;
    void setTree(Tree);
    Tree getTree() const;
};

}
