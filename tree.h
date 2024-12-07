#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <optional>
#include <vector>
#include <filesystem>
#include <functional>
#include "object.h"
#include "hash.h"
#include "blob.h"
#include "treediff.h"

namespace myvc {

namespace fs = std::filesystem;

class Tree : public Object {
public:
    class Provider {
    public:
        virtual std::optional<Blob> getBlob(const Hash &) const = 0;
        virtual std::optional<Tree> getTree(const Hash &) const = 0;
        virtual ~Provider() {};
    };

    class Node : public Serializable {
    private:
        Hash dataHash;
        bool blob;
        std::weak_ptr<Provider> prov;

    public:
        explicit Node(Hash dataHash = {}, bool blob = false);

        void write(std::ostream &) const override;
        void read(std::istream &) override;

        std::variant<Tree, Blob> getData() const;
        void setProvider(std::weak_ptr<Provider>);
    };

private:
    std::map<std::string, Node> nodes;
    std::weak_ptr<Provider> prov;

public:
    using Iterator = std::map<std::string, Node>::const_iterator;

    static TreeDiff diff(const Tree &, const Tree &);

    explicit Tree(std::map<std::string, Node> nodes = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    const std::map<std::string, Node> &getNodes() const;
    std::map<fs::path, Blob> getAllFiles() const;
    std::optional<std::variant<Tree, Blob>> getAtPath(const fs::path &) const;
    void setProvider(std::weak_ptr<Provider>);

    Iterator begin() const;
    Iterator end() const;
};

}
