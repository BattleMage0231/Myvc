#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <optional>
#include "object.h"
#include "hash.h"
#include "diff.h"

namespace myvc {

class Blob : public Object {
public:
    class Provider {
    public:
        virtual void createBlob(const Blob &) = 0;
        virtual ~Provider() {};
    };

private:
    std::vector<char> data;
    std::shared_ptr<Provider> prov;

public:
    static Diff diff(const Blob &, const Blob &);

    explicit Blob(std::vector<char> data = {}, std::shared_ptr<Provider> prov = {});
    explicit Blob(std::istream &, std::shared_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;
    void store() override;

    std::vector<char> &getData();
    const std::vector<char> &getData() const;
    void setProvider(std::shared_ptr<Provider>);
};

}
