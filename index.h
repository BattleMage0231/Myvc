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
#include "blob.h"
#include "treediff.h"
#include "serialize.h"

namespace myvc {

namespace fs = std::filesystem;

class Index : public Serializable {
public:
    class Provider {
    public:
        virtual std::optional<Tree> getTree(const Hash &) const = 0;
        //virtual void createTree(const Tree &) = 0;
        virtual std::optional<Blob> getBlob(const Hash &) const = 0;
        //virtual void createBlob(const Blob &) = 0;
        virtual ~Provider() {};
    };

private:
    Hash baseHash, treeHash;
    std::weak_ptr<Provider> prov;

public:
    explicit Index(Hash baseHash = {}, Hash treeHash = {}, std::weak_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    /*
    void updateEntry(const fs::path &, const Tree &);
    void updateEntry(const fs::path &, const Blob &);
    void deleteEntry(const fs::path &);
    void reset(Hash);

    void updateTree(Hash);
    void updateBase(Hash);
    */
    
    Tree getBase() const;
    Tree getTree() const;
    TreeDiff getDiff() const;
    void setProvider(std::weak_ptr<Provider>);
};

}
