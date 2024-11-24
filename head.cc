module;

#include <iostream>
#include <stdexcept>
#include <variant>

export module head;

import hash;
import writable;
import branch;
import commit;

namespace myvc {

using std::string, std::variant, std::holds_alternative, std::get;

using HeadState = variant<Hash, string>;

export class Head : public Writable {
    HeadState state;

    void write(std::ostream &out) const noexcept override {
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

    static Head read(std::istream &in) {
        string s, t; Hash h;
        if(s == "Head" && t == "Branch") {
            in >> s;
            return Head {s};
        } else if(s == "Head" && t == "Commit") {
            in >> h;
            return Head {h};
        }
        throw std::invalid_argument {"Head::read() failed parsing"};
    }

    bool isBranch() const noexcept {
        return holds_alternative<string>(state);
    }

    const HeadState &operator*() const noexcept {
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
