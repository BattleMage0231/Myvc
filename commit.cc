#include "commit.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

Commit::Commit(Hash parentHash, Hash treeHash, time_t time, std::string msg, std::shared_ptr<Provider> prov)
    : parentHash {parentHash}, treeHash {treeHash}, time {time}, msg {std::move(msg)}, prov {std::move(prov)} {}

Commit::Commit(std::istream &in, std::shared_ptr<Provider> prov) : prov {prov} {
    read(in);
}

void Commit::write(std::ostream &out) const {
    out << parentHash << treeHash;
    write_raw(out, time);
    write_string(out, msg);
}

void Commit::read(std::istream &in) {
    in >> parentHash >> treeHash;
    read_raw(in, time);
    read_string(in, msg);
}

void Commit::store() {
    prov->createCommit(*this);
}

Hash Commit::getHash() const {
    throw not_implemented {};
}

Commit Commit::getParent() const {
    return prov->getCommit(parentHash);
}

void Commit::setParent(Hash hash) {
    parentHash = hash;
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
    this->prov = prov;
}
