export module branch;

import <iostream>;
import <memory>;
import <stdexcept>;

export import hash;
export import writable;

namespace myvc {

namespace fs = std::filesystem;
using std::string, std::unique_ptr, std::make_unique, std::ostream, std::istream, std::invalid_argument;

export class Branch : public Writable {
    string name;
    Hash commitHash;

    void write(ostream &out) const noexcept override {
        out << "Branch" << '\n' << name << '\n' << commitHash << '\n';
    }

    fs::path getPath(const fs::path &base) const override {
        return base / "refs" / name;
    }

public:
    Branch(const string &name, const Hash &commitHash) : name {name}, commitHash {commitHash} {
        if(name.find('\n') != string::npos) throw invalid_argument {"Branch::Branch() called with newline in name"};
    }

    static unique_ptr<Branch> read(istream &in) {
        string s; Hash h;
        in >> s;
        if(s == "Branch") {
            in >> s >> h;
            return make_unique<Branch>(s, h);
        }
        throw invalid_argument {"Branch::read() failed parsing"};
    }

    const Hash &operator*() const {
        return commitHash;
    }

    void move(const Hash &newHash) {
        commitHash = newHash;
    }

    const string &getName() const {
        return name;
    }
};

}
