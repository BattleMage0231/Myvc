#include "treediff.h"
#include "errors.h"

using namespace myvc;

TreeChange::TreeChange(Type type, Blob oldBlob, Blob newBlob)
    : type {type}, oldBlob {std::move(oldBlob)}, newBlob {std::move(newBlob)} {}

bool TreeChange::operator==(const TreeChange &other) const {
    if(type != other.type) return false;
    if(type == Type::Add) {
        return newBlob == other.newBlob;
    } else if(type == Type::Delete) {
        return oldBlob == other.oldBlob;
    } else {
        return newBlob == other.newBlob && oldBlob == other.oldBlob;
    }
}

TreeDiff TreeDiff::merge(const TreeDiff &a, const TreeDiff &b) {
    throw not_implemented {};
}

TreeDiff::TreeDiff(const std::map<fs::path, Blob> &base, const std::map<fs::path, Blob> &other) {
    for(const auto &[path, blob] : base) {
        if(other.find(path) == other.end()) {
            changes.insert_or_assign(path, TreeChange {TreeChange::Type::Delete, blob});
        } else {
            if(blob != other.at(path)) {
                changes.insert_or_assign(path, TreeChange {TreeChange::Type::Modify, other.at(path), blob});
            }
        }
    }
    for(const auto &[path, blob] : other) {
        if(base.find(path) == base.end()) {
            changes.insert_or_assign(path, TreeChange {TreeChange::Type::Add, Blob {}, blob});
        }
    }
}

std::map<fs::path, TreeChange> &TreeDiff::getChanges() {
    return changes;
}

const std::map<fs::path, TreeChange> &TreeDiff::getChanges() const {
    return changes;
}
