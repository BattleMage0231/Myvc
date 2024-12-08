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

std::pair<TreeDiff, TreeDiff::Conflicts> TreeDiff::merge(const TreeDiff &a, const TreeDiff &b) {
    std::map<fs::path, TreeChange> changes = a.getChanges();
    std::set<fs::path> deleteConflicts;
    std::map<fs::path, std::pair<Diff, Diff::Conflicts>> modifyConflicts;
    for(const auto &[path, change] : b.getChanges()) {
        if(changes.find(path) == changes.end()) {
            changes.insert_or_assign(path, change);
            continue;
        }
        auto otherChange = changes.at(path);
        if(change.type == TreeChange::Type::Modify) {
            if(otherChange.type == TreeChange::Type::Modify) {
                Diff bDiff = Blob::diff(change.oldBlob, change.newBlob);
                Diff aDiff = Blob::diff(otherChange.oldBlob, otherChange.newBlob);
                auto res = Diff::merge(bDiff, aDiff);
                if(res.second.empty()) {
                    changes.at(path) = TreeChange { TreeChange::Type::Modify, change.oldBlob, Blob {res.first.apply()} };
                } else {
                    changes.erase(path);
                    modifyConflicts.insert_or_assign(path, res);
                }
            } else {
                changes.erase(path);
                deleteConflicts.insert(path);
            }
        } else if(change.type == TreeChange::Type::Add) {
            if(otherChange.type == TreeChange::Type::Add) {
                Diff bDiff = Blob::diff(change.oldBlob, change.newBlob);
                Diff aDiff = Blob::diff(otherChange.oldBlob, otherChange.newBlob);
                auto res = Diff::merge(bDiff, aDiff);
                if(res.second.empty()) {
                    changes.at(path) = TreeChange { TreeChange::Type::Add, change.oldBlob, Blob {res.first.apply()} };
                } else {
                    changes.erase(path);
                    modifyConflicts.insert_or_assign(path, res);
                }
            } else {
                changes.erase(path);
                deleteConflicts.insert(path);
            }
        } else {
            if(otherChange.type == TreeChange::Type::Modify || otherChange.type == TreeChange::Type::Add) {
                changes.erase(path);
                deleteConflicts.insert(path);
            }
        }
    }
    return { TreeDiff {std::move(changes)}, Conflicts { std::move(deleteConflicts), std::move(modifyConflicts) } };
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

TreeDiff::TreeDiff(std::map<fs::path, TreeChange> changes) : changes {std::move(changes)} {}

std::map<fs::path, TreeChange> &TreeDiff::getChanges() {
    return changes;
}

const std::map<fs::path, TreeChange> &TreeDiff::getChanges() const {
    return changes;
}

bool TreeDiff::empty() const {
    return changes.empty();
}

TreeDiff::Iterator TreeDiff::begin() const {
    return changes.begin();
}

TreeDiff::Iterator TreeDiff::end() const {
    return changes.end();
}
