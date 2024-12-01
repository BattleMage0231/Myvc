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
        for(const auto &c : aCur) aPool.insert(c.getHash());
        for(const auto &c : bCur) bPool.insert(c.getHash());
        // check intersections
        std::set<Hash> sect;
        std::set_intersection(aPool.begin(), aPool.end(), bPool.begin(), bPool.end(), std::inserter(sect, sect.begin()));
        if(!sect.empty()) {
            Hash first = *sect.begin();
            for(const auto &c : aCur) {
                if(c.getHash() == first) return c;
            }
            for(const auto &c : bCur) {
                if(c.getHash() == first) return c;
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

std::vector<Commit> Commit::getAllParents(const Commit &c) {
    std::vector<Commit> res;
    for(const auto &parent : c.getParents()) {
        res.emplace_back(parent);
        auto nested = getAllParents(parent);
        res.insert(res.end(), nested.begin(), nested.end());
    }
    return res;
}

Commit::Commit(std::set<Hash> parentHashes, Hash treeHash, time_t time, std::string msg, std::shared_ptr<Provider> prov)
    : parentHashes {std::move(parentHashes)}, treeHash {treeHash}, time {time}, msg {std::move(msg)}, prov {std::move(prov)} {}

Commit::Commit(std::istream &in, std::shared_ptr<Provider> prov) : prov {prov} {
    read(in);
}

void Commit::write(std::ostream &out) const {
    write_raw(out, parentHashes.size());
    for(const auto &h : parentHashes) out << h;
    out << treeHash;
    write_raw(out, time);
    write_string(out, msg);
}

void Commit::read(std::istream &in) {
    size_t sz;
    read_raw(in, sz);
    parentHashes.clear();
    for(size_t i = 0; i < sz; ++i) {
        Hash h;
        in >> h;
        parentHashes.insert(std::move(h));
    }
    in >> treeHash;
    read_raw(in, time);
    read_string(in, msg);
}

void Commit::store() {
    prov->createCommit(*this);
}

std::vector<Commit> Commit::getParents() const {
    std::vector<Commit> commits;
    for(const auto &h : parentHashes) commits.emplace_back(prov->getCommit(h));
    return commits;
}

std::set<Hash> &Commit::getParentHashes() {
    return parentHashes;
}

const std::set<Hash> &Commit::getParentHashes() const {
    return parentHashes;
}

Tree Commit::getTree() const {
    return prov->getTree(treeHash);
}

void Commit::setTree(Hash hash) {
    treeHash = hash;
}

time_t Commit::getTime() const {
    return time;
}

void Commit::setTime(time_t time) {
    this->time = time;
}

const std::string &Commit::getMsg() const {
    return msg;
}

void Commit::setMsg(std::string str) {
    msg = std::move(str);
}

void Commit::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
