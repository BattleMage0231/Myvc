#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include "object.h"
#include "hash.h"
#include "blob.h"

namespace myvc {

class Tree : public Object {
public:
    class Provider {
    public:
        virtual Blob getBlob(Hash) const = 0;
        virtual Tree getTree(Hash) const = 0;
        virtual void createTree(const Tree &) = 0;
        virtual ~Provider() {};
    };

    class Node : public Serializable {
    private:
        Hash dataHash;
        bool blob;
        std::weak_ptr<Provider> prov;

    public:
        explicit Node(Hash dataHash = {}, bool blob = false, std::weak_ptr<Provider> prov = {});
        explicit Node(std::istream &, std::weak_ptr<Provider> prov = {});

        void write(std::ostream &) const override;
        void read(std::istream &) override;

        bool isBlob() const;
        std::variant<Tree, Blob> getData() const;
        std::variant<Tree, Blob> operator*() const;
        void setBlob(Hash);
        void setTree(Hash);
        void setProvider(std::weak_ptr<Provider>);
    };

private:
    std::map<std::string, Node> nodes;
    std::shared_ptr<Provider> prov;

public:
    using Iterator = std::map<std::string, Node>::const_iterator;

    explicit Tree(std::map<std::string, Node> nodes = {}, std::shared_ptr<Provider> prov = {});
    explicit Tree(std::istream &, std::shared_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;
    void store() override;

    std::map<std::string, Node> &getNodes();
    const std::map<std::string, Node> &getNodes() const;
    void setProvider(std::shared_ptr<Provider>);

    Iterator begin() const;
    Iterator end() const;
};

}
