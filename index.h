#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <ctime>
#include <filesystem>
#include "object.h"
#include "hash.h"
#include "tree.h"
#include "blob.h"

namespace myvc {

namespace fs = std::filesystem;

class Index : public Synced {
public:
    class Provider {
    public:
        virtual Index getIndex() const = 0;
        virtual void updateIndex(const Index &) = 0;
        virtual Blob getBlob(Hash) const = 0;
        virtual void createBlob(const Blob &) = 0;
        virtual ~Provider() {};
    };

private:
    std::map<fs::path, Hash> blobs;
    std::unique_ptr<Provider> prov;

public:
    explicit Index(std::map<fs::path, Hash> blobs = {}, std::unique_ptr<Provider> prov = {});
    explicit Index(std::unique_ptr<Provider>);
    Index(const Index &);
    Index &operator=(const Index &);

    std::vector<char> serialize() const override;
    void deserialize(std::vector<char>) override;
    void reload() override;
    void store() override;

    void addFile(fs::path, Hash);
    void removeFile(const fs::path &);
    Blob getFile(const fs::path &) const;
    Tree applyChanges(const Tree &) const;
    void setProvider(std::unique_ptr<Provider>);
};

}
