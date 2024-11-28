#pragma once

#include <iostream>
#include <set>
#include <memory>
#include <string>
#include <ctime>
#include "synced.h"
#include "hash.h"
#include "commit.h"

namespace myvc {

class Branch : public Synced {
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
    std::unique_ptr<Provider> prov;

public:
    explicit Branch(std::string name = {}, Hash commitHash = {}, std::unique_ptr<Provider> prov = {});
    explicit Branch(std::string, std::unique_ptr<Provider>);
    Branch(const Branch &);
    Branch &operator=(const Branch &);

    std::vector<char> serialize() const override;
    void deserialize(std::vector<char>) override;
    void reload() override;
    void store() override;

    const std::string &getName() const;
    void setName(std::string);
    Commit getCommit() const;
    void setCommit(Hash);
    Commit operator*() const;
    void setProvider(std::unique_ptr<Provider>);
};

}
