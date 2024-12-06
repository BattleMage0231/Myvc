#pragma once

#include <iostream>
#include <set>
#include <memory>
#include <string>
#include <ctime>
#include <optional>
#include <functional>
#include "serialize.h"
#include "hash.h"
#include "commit.h"

namespace myvc {

class Branch : public Serializable {
public:
    class Provider {
    public:
        virtual std::optional<Commit> getCommit(const Hash &) const = 0;
        virtual ~Provider() {};
    };

private:
    std::string name;
    Hash commitHash;
    std::weak_ptr<Provider> prov;

public:
    explicit Branch(std::string name = {}, Hash commitHash = {}, std::weak_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    const std::string &getName() const;
    Commit getCommit() const;
    void setCommit(Hash);
    void setProvider(std::weak_ptr<Provider>);
};

}
