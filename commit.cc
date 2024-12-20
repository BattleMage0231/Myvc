#include <algorithm>
#include <iterator>
#include "commit.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

Commit Commit::getLCA(const Commit &a, const Commit &b) {
    std::set<Hash> aPool, bPool;
    std::vector<Commit> aCur {a}, bCur {b};
    while(!aCur.empty() || !bCur.empty()) {
        for(const auto &c : aCur) aPool.insert(c.hash());
        for(const auto &c : bCur) bPool.insert(c.hash());
        // check intersections
        std::set<Hash> sect;
        std::set_intersection(aPool.begin(), aPool.end(), bPool.begin(), bPool.end(), std::inserter(sect, sect.begin()));
        if(!sect.empty()) {
            Hash first = *sect.begin();
            for(const auto &c : aCur) {
                if(c.hash() == first) return c;
            }
            for(const auto &c : bCur) {
                if(c.hash() == first) return c;
            }
            THROW("impossible");
        }
        // get next list
        std::vector<Commit> aNext, bNext;
        for(const auto &c : aCur) {
            auto parents = c.getParents();
            aNext.insert(aNext.end(), parents.begin(), parents.end());
        }
        for(const auto &c : bCur) {
            auto parents = c.getParents();
            bNext.insert(bNext.end(), parents.begin(), parents.end());
        }
        aCur = std::move(aNext);
        bCur = std::move(bNext);
    }
    THROW("no ancestors");
}

std::set<Commit> Commit::getAllReachable(const Commit &c) {
    std::set<Commit> res {c};
    for(const auto &parent : c.getParents()) {
        auto nested = getAllReachable(parent);
        res.insert(nested.begin(), nested.end());
    }
    return res;
}

Commit::Commit(std::set<Hash> parentHashes, Hash treeHash, time_t time, std::string msg)
    : parentHashes {std::move(parentHashes)}, treeHash {std::move(treeHash)}, time {time}, msg {std::move(msg)} {}

void Commit::write(std::ostream &out) const {
    write_raw(out, parentHashes.size());
    for(const auto &h : parentHashes) write_hash(out, h);
    write_hash(out, treeHash);
    write_raw(out, time);
    write_string(out, msg);
}

void Commit::read(std::istream &in) {
    size_t sz;
    read_raw(in, sz);
    parentHashes.clear();
    for(size_t i = 0; i < sz; ++i) {
        Hash h;
        read_hash(in, h);
        parentHashes.insert(std::move(h));
    }
    read_hash(in, treeHash);
    read_raw(in, time);
    read_string(in, msg);
}

std::vector<Commit> Commit::getParents() const {
    if(!prov.lock()) THROW("nonexistent provider");
    std::vector<Commit> commits;
    for(const auto &h : parentHashes) commits.emplace_back(prov.lock()->getCommit(h).value());
    return commits;
}

const std::set<Hash> &Commit::getParentHashes() const {
    return parentHashes;
}

Tree Commit::getTree() const {
    if(!prov.lock()) THROW("nonexistent provider");
    return prov.lock()->getTree(treeHash).value();
}

time_t Commit::getTime() const {
    return time;
}

const std::string &Commit::getMsg() const {
    return msg;
}

bool Commit::hasParent(const Commit &c) const {
    if(*this == c) return true;
    for(const Commit &par : getParents()) {
        if(par.hasParent(c)) return true;
    }
    return false;
}

std::vector<Commit> Commit::getParentChain(const Commit &c) const {
    if(*this == c) return {c};
    for(const Commit &par : getParents()) {
        std::vector<Commit> chain = par.getParentChain(c);
        if(!chain.empty()) {
            chain.emplace_back(*this);
            return chain;
        }
    }
    return {};
}

void Commit::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
