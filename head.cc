#include "head.h"

using namespace myvc;

Head::Head(std::variant<std::string, Hash> state, std::shared_ptr<Provider> prov)
    : state {std::move(state)}, prov {std::move(prov)} {}

void Head::write(std::ostream &out) const {
    if(isBranch()) {
        write_raw(out, true);
        write_string(out, std::get<std::string>(state));
    } else{
        write_raw(out, false);
        write_hash(out, std::get<Hash>(state));
    }
}

void Head::read(std::istream &in) {
    bool isBranch;
    read_raw(in, isBranch);
    if(isBranch) {
        std::string s;
        read_string(in, s);
        state = std::move(s);
    } else {
        Hash h;
        read_hash(in, h);
        state = std::move(h);
    }
}

void Head::reload() {
    *this = prov->getHead().value();
}

void Head::store() {
    prov->updateHead(*this);
}

bool Head::isBranch() const {
    return std::holds_alternative<std::string>(state);
}

std::optional<Branch> Head::getBranch() const {
    if(isBranch()) {
        return prov->getBranch(std::get<std::string>(state)).value();
    } else {
        return {};
    }
}

Commit Head::getCommit() const {
    auto branch = getBranch();
    if(branch) {
        return *(branch.value());
    } else {
        return prov->getCommit(std::get<Hash>(state)).value();
    }
}

Commit Head::operator*() const {
    return getCommit();
}

void Head::setState(std::variant<std::string, Hash> state) {
    this->state = std::move(state);
}

void Head::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
