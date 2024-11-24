export module object;

import hash;
import writable;

namespace myvc {

export class Object : public Writable {
public:
    virtual ~Object() {}

    virtual Hash getHash() const = 0;
};

}
