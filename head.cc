#include "head.h"

using namespace myvc;

Head::Head(std::variant<std::monostate, std::string, Hash> state)
    : state {std::move(state)} {}

void Head::write(std::ostream &out) const {
    if(std::holds_alternative<std::string>(state)) {
        write_raw(out, true);
        write_string(out, std::get<std::string>(state));
    } else if(std::holds_alternative<Hash>(state)) {
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

bool Head::hasState() const {
    return !std::holds_alternative<std::monostate>(state);
}

std::variant<const std::reference_wrapper<Branch>, Commit> Head::get() const {
    if(std::holds_alternative<std::string>(state)) {
        return prov.lock()->getBranch(std::get<std::string>(state)).value();
    } else {
        return prov.lock()->getCommit(std::get<Hash>(state)).value();
    }
}

Commit Head::getCommit() const {
    auto val = get();
    if(std::holds_alternative<const std::reference_wrapper<Branch>>(val)) {
        return std::get<const std::reference_wrapper<Branch>>(val).get().getCommit();
    } else {
        return std::get<Commit>(val);
    }
}

void Head::setBranch(std::string branch) {
    this->state = std::move(branch);
}

void Head::setCommit(Hash commitHash) {
    this->state = std::move(commitHash);
}

void Head::setProvider(std::weak_ptr<Provider> prov) {
    this->prov = std::move(prov);
}
