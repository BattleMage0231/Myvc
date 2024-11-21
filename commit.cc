export module commit;

export import <memory>;
import <stdexcept>;

export import object;

namespace myvc {

using std::shared_ptr, std::make_shared, std::logic_error;

export class Commit : public Object {
    shared_ptr<Commit> parent;

public:
    Commit(shared_ptr<Commit> parent = make_shared<Commit>(nullptr)) : parent {parent} {}

    bool isRoot() const {
        return static_cast<bool>(parent);
    }

    Commit &getParent() {
        if(isRoot()) throw logic_error {"Commit::getParent() called on root node"};
        return *parent;
    }

    const Commit &getParent() const {
        if(isRoot()) throw logic_error {"Commit::getParent() called on root node"};
        return *parent;
    }
};

}
