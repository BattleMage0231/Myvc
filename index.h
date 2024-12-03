#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <optional>
#include <filesystem>
#include "object.h"
#include "hash.h"
#include "tree.h"
#include "blob.h"
#include "treediff.h"

namespace myvc {

namespace fs = std::filesystem;

class Index : public Stored {
public:
    class Provider {
    public:
        virtual std::optional<Index> getIndex() const = 0;
        virtual void updateIndex(const Index &) = 0;
        virtual std::optional<Tree> getTree(Hash) const = 0;
        virtual void createTree(const Tree &) = 0;
        virtual std::optional<Blob> getBlob(Hash) const = 0;
        virtual void createBlob(const Blob &) = 0;
        virtual ~Provider() {};
    };

private:
    Hash baseHash, treeHash;
    std::shared_ptr<Provider> prov;

public:
    explicit Index(Hash baseHash = {}, Hash treeHash = {}, std::shared_ptr<Provider> prov = {});
    explicit Index(std::istream &, std::shared_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;
    void reload() override;
    void store() override;

    void updateEntry(const fs::path &, const Tree &);
    void updateEntry(const fs::path &, const Blob &);
    void deleteEntry(const fs::path &);
    void reset(Hash);
    void updateBase(Hash);
    Tree getBase() const;
    Tree getTree() const;
    TreeDiff getDiff() const;
    void setProvider(std::shared_ptr<Provider>);
};

}
