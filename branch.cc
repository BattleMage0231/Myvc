#include "branch.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

Branch::Branch(std::string name, Hash commitHash, std::shared_ptr<Provider> prov)
    : name {std::move(name)}, commitHash {commitHash}, prov {prov} {}

Branch::Branch(std::istream &in, std::shared_ptr<Provider> prov) : prov {prov} {
    read(in);
}

void Branch::write(std::ostream &out) const {
    write_string(out, name);
    out << commitHash;
}

void Branch::read(std::istream &in) {
    read_string(in, name);
    in >> commitHash;
}

void Branch::reload() {
    *this = prov->getBranch(name);
}

void Branch::store() {
    prov->updateBranch(*this);
}

const std::string &Branch::getName() const {
    return name;
}

void Branch::setName(std::string name) {
    this->name = std::move(name);
}

Commit Branch::getCommit() const {
    return prov->getCommit(commitHash);
}

void Branch::setCommit(Hash hash) {
    commitHash = hash;
}

Commit Branch::operator*() const {
    return getCommit();
}

void Branch::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
