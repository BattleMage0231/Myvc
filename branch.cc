#include "branch.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

Branch::Branch(std::string name, Hash commitHash)
    : name {std::move(name)}, commitHash {std::move(commitHash)} {}

void Branch::write(std::ostream &out) const {
    write_string(out, name);
    write_hash(out, commitHash);
}

void Branch::read(std::istream &in) {
    read_string(in, name);
    read_hash(in, commitHash);
}

const std::string &Branch::getName() const {
    return name;
}

Commit Branch::getCommit() const {
    return prov.lock()->getCommit(commitHash).value();
}

void Branch::setCommit(Hash hash) {
    commitHash = std::move(hash);
}

void Branch::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
