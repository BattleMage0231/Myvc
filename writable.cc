module;

#include <iostream>
#include <filesystem>

export module writable;

namespace myvc {

namespace fs = std::filesystem;
using std::ostream, std::ofstream;

export class Writable {
    virtual void write(ostream &) const noexcept = 0;

public:
    virtual ~Writable() {}

    friend ostream &operator<<(ostream &out, const Writable &w) noexcept {
        w.write(out);
        return out;
    }
};

}
