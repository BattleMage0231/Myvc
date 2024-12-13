#pragma once

#include <iostream>
#include <set>
#include <memory>
#include <string>
#include <ctime>
#include <optional>
#include <functional>
#include "object.h"
#include "hash.h"
#include "tree.h"

namespace myvc {

class Commit : public Object {
public:
    class Provider {
    public:
        virtual std::optional<Commit> getCommit(const Hash &) const = 0;
        virtual std::optional<Tree> getTree(const Hash &) const = 0;
        virtual ~Provider() {};
    };

private:
    std::set<Hash> parentHashes;
    Hash treeHash;
    time_t time;
    std::string msg;
    std::weak_ptr<Provider> prov;

public:
    static Commit getLCA(const Commit &, const Commit &);
    static std::set<Commit> getAllReachable(const Commit &);

    explicit Commit(std::set<Hash> parentHashes = {}, Hash treeHash = {}, time_t time = {}, std::string msg = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    std::vector<Commit> getParents() const;
    const std::set<Hash> &getParentHashes() const;
    Tree getTree() const;
    time_t getTime() const;
    const std::string &getMsg() const;
    bool hasParent(const Commit &) const;
    std::vector<Commit> getParentChain(const Commit &) const;
    void setProvider(std::weak_ptr<Provider>);
};

}
