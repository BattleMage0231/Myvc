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
    std::shared_ptr<Provider> prov;

public:
    using Iterator = int;
    using ConstIterator = int;

    explicit Tree(std::map<std::string, Node> nodes = {}, std::shared_ptr<Provider> prov = {});
    explicit Tree(std::istream &, std::shared_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;
    void store() override;

    Hash getHash() const override;

    void setProvider(std::shared_ptr<Provider>);

    Iterator begin();
    Iterator end();
    ConstIterator begin() const;
    ConstIterator end() const;
};

}
