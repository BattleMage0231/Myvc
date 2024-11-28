#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "object.h"
#include "hash.h"

namespace myvc {

class Blob : public Object {
public:
    class Provider {
    public:
        virtual Blob getBlob(Hash) const = 0;
        virtual void createBlob(const Blob &) = 0;
        virtual ~Provider() {};
    };

private:
    std::vector<char> data;
    std::unique_ptr<Provider> prov;

public:
    explicit Blob(std::vector<char> data = {}, std::unique_ptr<Provider> prov = {});
    explicit Blob(Hash, std::unique_ptr<Provider>);
    Blob(const Blob &);
    Blob &operator=(const Blob &);

    std::vector<char> serialize() const override;
    void deserialize(std::vector<char>) override;
    void store() override;

    Hash getHash() const override;

    std::vector<char> &getData();
    const std::vector<char> &getData() const;
    void setProvider(std::unique_ptr<Provider>);
};

}
