export module object;

import hash;
import writable;

namespace myvc {

export class Object : public Writable {
public:
    virtual ~Object() {}

    virtual Hash getHash() const noexcept = 0;

    auto operator<=>(const Object &other) const noexcept {
        return getHash() <=> other.getHash();
    }

    bool operator==(const Object &) const noexcept = default;
};

}
