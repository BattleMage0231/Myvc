#pragma once

#include <iostream>
#include <set>
#include <memory>
#include <string>
#include <ctime>
#include "stored.h"
#include "hash.h"
#include "commit.h"

namespace myvc {

class Branch : public Stored {
public:
    class Provider {
    public:
        virtual Branch getBranch(const std::string &) const = 0;
        virtual void updateBranch(const Branch &) = 0;
        virtual ~Provider() {};
    };

private:
    std::string name;
    Hash commitHash;
    std::shared_ptr<Provider> prov;

public:
    explicit Branch(std::string name = {}, Hash commitHash = {}, std::shared_ptr<Provider> prov = {});
    explicit Branch(std::istream &, std::shared_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;
    void reload() override;
    void store() override;

    const std::string &getName() const;
    void setName(std::string);
    Commit getCommit() const;
    void setCommit(Hash);
    Commit operator*() const;
    void setProvider(std::shared_ptr<Provider>);
};

}
