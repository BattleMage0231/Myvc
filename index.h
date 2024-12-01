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

namespace myvc {

namespace fs = std::filesystem;

class Index : public Stored {
public:
    class Provider {
    public:
        virtual std::optional<Index> getIndex() const = 0;
        virtual void updateIndex(const Index &) = 0;
        virtual std::optional<Blob> getBlob(Hash) const = 0;
        virtual void createBlob(const Blob &) = 0;
        virtual ~Provider() {};
    };

private:
    std::map<fs::path, Hash> blobs;
    std::shared_ptr<Provider> prov;

public:
    explicit Index(std::map<fs::path, Hash> blobs = {}, std::shared_ptr<Provider> prov = {});
    explicit Index(std::istream &, std::shared_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;
    void reload() override;
    void store() override;

    void addFile(fs::path, Hash);
    void removeFile(const fs::path &);
    std::optional<Blob> getFile(const fs::path &) const;
    Tree applyChanges(const Tree &) const;
    void setProvider(std::shared_ptr<Provider>);
};

}
