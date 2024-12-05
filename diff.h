#pragma once

#include <string>
#include <vector>
#include <set>
#include <utility>

namespace myvc {

struct Change {
    enum class Type { Add, Delete };

    Type type;
    std::string content;

    bool operator==(const Change &) const = default;
};

class Hunk {
    size_t index, end;
    std::vector<Change> changes;

public:
    Hunk(size_t, std::vector<Change> changes = {});

    std::vector<std::string> getOurs() const;
    std::vector<std::string> getTheirs() const;

    size_t getIndex() const;
    size_t getEnd() const;
    const std::vector<Change> &getChanges() const;

    auto operator<=>(const Hunk &) const = default;
    bool operator==(const Hunk &) const = default;
};

class Diff {
    std::vector<std::string> base;
    std::vector<Hunk> hunks;

public:
    using Conflicts = std::vector<std::pair<Hunk, Hunk>>;

    static std::pair<Diff, Conflicts> merge(const Diff &, const Diff &);

    Diff(std::vector<std::string>, const std::vector<std::string> &);

    Diff(std::vector<std::string>, std::vector<Hunk>);

    std::vector<std::string> apply() const;

    const std::vector<std::string> &getBase() const;
    const std::vector<Hunk> &getHunks() const;
};

}
