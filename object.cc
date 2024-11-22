export module object;

import hash;
import writable;

namespace myvc {

namespace fs = std::filesystem;

export class Object : public Writable {
    fs::path getPath(const fs::path &base) const override {
        return base / "objects" / getHash();
    }

public:
    virtual ~Object() {}

    virtual const Hash &getHash() const = 0;
};

}
