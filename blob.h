#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <optional>
#include "object.h"
#include "hash.h"
#include "diff.h"

namespace myvc {

class Blob : public Object {
    std::vector<char> data;

public:
    static Diff diff(const Blob &, const Blob &);

    explicit Blob(std::vector<char> data = {});
    explicit Blob(const std::vector<std::string> &);

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    const std::vector<char> &getData() const;

    bool operator==(const Blob &) const;
};

}
