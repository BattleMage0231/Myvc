#pragma once

#include <filesystem>
#include "tree.h"

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
    static TreeDiff merge(const TreeDiff &, const TreeDiff &);

    TreeDiff(const std::map<fs::path, Blob> &, const std::map<fs::path, Blob> &);

    std::map<fs::path, TreeChange> &getChanges();
    const std::map<fs::path, TreeChange> &getChanges() const;
};

}
