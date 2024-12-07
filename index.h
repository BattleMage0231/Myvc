#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <optional>
#include <filesystem>
#include <functional>
#include "hash.h"
#include "tree.h"
#include "serialize.h"

namespace myvc {

namespace fs = std::filesystem;

class Index : public Serializable {
public:
    class Provider {
    public:
        virtual std::optional<Tree> getTree(const Hash &) const = 0;
        virtual ~Provider() {};
    };

private:
    Hash treeHash;
    std::weak_ptr<Provider> prov;

public:
    explicit Index(Hash treeHash = {}, std::weak_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    Tree getTree() const;
    void setTree(const Hash &);
    void setProvider(std::weak_ptr<Provider>);
};

}
