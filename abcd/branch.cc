module;

#include <iostream>
#include <stdexcept>

export module branch;

import hash;
import writable;

namespace myvc {

using std::string;

export class Branch : public Writable {
    string name;
    Hash commitHash;

    void write(std::ostream &out) const noexcept override {
        out << "Branch" << '\n' << name << '\n' << commitHash << '\n';
    }

public:
    Branch(string name, Hash commitHash) : name {std::move(name)}, commitHash {std::move(commitHash)} {
        for(char c : name) {
            if(!(('a' <= c && c <= 'z') && ('a' <= c && c <= 'z'))) {
                throw std::invalid_argument {"Branch::Branch() called with newline in name"};
            }
        }
    }

    static Branch read(std::istream &in) {
        string s; Hash h;
        in >> s;
        if(s == "Branch") {
            in >> s >> h;
            return Branch {s, h};
        }
        throw std::invalid_argument {"Branch::read() failed parsing"};
    }

    const Hash &operator*() const {
        return commitHash;
    }

    void move(Hash newHash) {
        commitHash = std::move(newHash);
    }

    const string &getName() const {
        return name;
    }
};

}
