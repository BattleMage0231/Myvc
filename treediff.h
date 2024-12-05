#pragma once

#include <filesystem>
#include <map>
#include <set>
#include "blob.h"

namespace myvc {

namespace fs = std::filesystem;

struct TreeChange {
    enum class Type { Add, Delete, Modify };

    Type type;
    Blob oldBlob, newBlob;

    explicit TreeChange(Type type, Blob oldBlob = Blob {}, Blob newBlob = Blob {});

    bool operator==(const TreeChange &) const;
};

class TreeDiff {
    std::map<fs::path, TreeChange> changes;

public:
    struct Conflicts {
        std::set<fs::path> deleteConflicts;
        std::map<fs::path, std::pair<Diff, Diff::Conflicts>> modifyConflicts;
    };

    static std::pair<TreeDiff, Conflicts> merge(const TreeDiff &, const TreeDiff &);

    TreeDiff(const std::map<fs::path, Blob> &, const std::map<fs::path, Blob> &);
    TreeDiff(std::map<fs::path, TreeChange>);

    std::map<fs::path, TreeChange> &getChanges();
    const std::map<fs::path, TreeChange> &getChanges() const;
};

}
