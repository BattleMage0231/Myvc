#include <algorithm>
#include <iterator>
#include "commit.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

std::optional<Commit> Commit::getLCA(const Commit &a, const Commit &b) {
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
            throw not_implemented {};
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
    return {};
}

std::vector<Commit> Commit::getAllReachable(const Commit &c) {
    std::vector<Commit> res {c};
    for(const auto &parent : c.getParents()) {
        auto nested = getAllReachable(parent);
        res.insert(res.end(), nested.begin(), nested.end());
    }
    return res;
}

Commit::Commit(std::set<Hash> parentHashes, Hash treeHash, time_t time, std::string msg, std::weak_ptr<Provider> prov)
    : parentHashes {std::move(parentHashes)}, treeHash {std::move(treeHash)}, time {time}, msg {std::move(msg)}, prov {std::move(prov)} {}

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
    std::vector<Commit> commits;
    for(const auto &h : parentHashes) commits.emplace_back(prov.lock()->getCommit(h).value());
    return commits;
}

const std::set<Hash> &Commit::getParentHashes() const {
    return parentHashes;
}

Tree Commit::getTree() const {
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

void Commit::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
