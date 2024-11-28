#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include "object.h"
#include "hash.h"

namespace myvc {

class Tree : public Object {
public:
    class Provider {
    public:
        virtual Tree getTree(Hash) const = 0;
        virtual void createTree(const Tree &) = 0;
        virtual ~Provider() {};
    };

    struct Node : public Serializable {
        bool isBlob;
        Hash dataHash;

        void write(std::ostream &) const override;
        void read(std::istream &) override;
    };

private:
    std::map<std::string, Node> nodes;
    std::unique_ptr<Provider> prov;

public:
    explicit Tree(std::map<std::string, Node> nodes = {}, std::unique_ptr<Provider> prov = {});
    explicit Tree(Hash, std::unique_ptr<Provider>);
    explicit Tree(std::istream &, std::unique_ptr<Provider> prov = {});
    Tree(const Tree &);
    Tree &operator=(const Tree &);

    void write(std::ostream &) const override;
    void read(std::istream &) override;
    void store() override;

    Hash getHash() const override;

    void setProvider(std::unique_ptr<Provider>);
};

}
