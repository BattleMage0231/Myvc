export module branch;

import <memory>;

export import symbol;

namespace myvc {

using std::string, std::shared_ptr;

export class Branch : public Symbol {
    shared_ptr<Commit> ref;

public:
    Branch(const string &name, const shared_ptr<Commit> &ref) : 
        Symbol {name}, ref {ref} {}
    
    Commit &operator*() override {
        return *ref;
    }

    const Commit &operator*() const override {
        return *ref;
    }

    void move(const shared_ptr<Commit>& ref) {
        this->ref = ref;
    }
};

}
