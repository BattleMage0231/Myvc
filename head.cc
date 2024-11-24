module;

#include <iostream>
#include <memory>
#include <stdexcept>
#include <variant>

export module head;

export import hash;
export import writable;
export import branch;
export import commit;

namespace myvc {

using std::string, std::unique_ptr, std::make_unique, std::ostream, std::istream, std::invalid_argument, std::variant, std::reference_wrapper, std::holds_alternative, std::get;

using HeadState = variant<Hash, string>;

export class Head : public Writable {
    HeadState state;

    void write(ostream &out) const noexcept override {
        out << "Head" << '\n';
        if(isBranch()) {
            out << "Branch" << '\n' << get<string>(state) << '\n';
        } else {
            out << "Commit" << '\n' << get<Hash>(state) << '\n';
        }
    }

public:
    Head(string name) : state {std::move(name)} {}

    Head(Hash hash) : state {std::move(hash)} {}

    static Head read(istream &in) {
        string s, t; Hash h;
        if(s == "Head" && t == "Branch") {
            in >> s;
            return Head {s};
        } else if(s == "Head" && t == "Commit") {
            in >> h;
            return Head {h};
        }
        throw invalid_argument {"Head::read() failed parsing"};
    }

    bool isBranch() const {
        return holds_alternative<string>(state);
    }

    const HeadState &operator*() const {
        return state;
    }

    void move(Hash newCommit) {
        state = std::move(newCommit);
    }

    void move(string newBranch) {
        state = std::move(newBranch);
    }
};

}
