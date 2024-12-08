#include "repository.h"

using namespace myvc;

Repository::Repository(fs::path path) : path {path}, store {path} {}

Head &Repository::getHead() const {
    return store.getHead();
}

std::optional<std::reference_wrapper<Branch>> Repository::getBranch(const std::string &name) {
    return store.getBranch(name);
}

std::optional<Commit> Repository::getCommit(const Hash &hash) const {
    return store.getCommit(hash);
}

std::optional<Hash> Repository::resolvePartialHash(std::string partial) const {
    return store.resolvePartialObjectHash(std::move(partial));
}
