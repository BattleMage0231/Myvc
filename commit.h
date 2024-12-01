#pragma once

#include <iostream>
#include <set>
#include <memory>
#include <string>
#include <ctime>
#include <optional>
#include "object.h"
#include "hash.h"
#include "tree.h"

namespace myvc {

class Commit : public Object {
public:
    class Provider {
    public:
        virtual Commit getCommit(Hash) const = 0;
        virtual void createCommit(const Commit &) = 0;
        virtual Tree getTree(Hash) const = 0;
        virtual ~Provider() {};
    };

private:
    std::set<Hash> parentHashes;
    Hash treeHash;
    time_t time;
    std::string msg;
    std::shared_ptr<Provider> prov;

public:
    static std::optional<Commit> getLCA(const Commit &, const Commit &);
    static std::vector<Commit> getAllParents(const Commit &);

    explicit Commit(std::set<Hash> parentHashes = {}, Hash treeHash = {}, time_t time = {}, std::string msg = {}, std::shared_ptr<Provider> prov = {});
    explicit Commit(std::istream &, std::shared_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;
    void store() override;

    std::vector<Commit> getParents() const;
    std::set<Hash> &getParentHashes();
    const std::set<Hash> &getParentHashes() const;
    Tree getTree() const;
    void setTree(Hash);
    time_t getTime() const;
    void setTime(time_t);
    const std::string &getMsg() const;
    void setMsg(std::string);
    void setProvider(std::shared_ptr<Provider>);
};

}
