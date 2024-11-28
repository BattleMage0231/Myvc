module;

#include <iostream>

export module writable;

namespace myvc {

using std::ostream;

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
