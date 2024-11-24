module;

#include <iostream>
#include <memory>
#include <chrono>
#include <stdexcept>
#include <vector>

export module commit;

export import object;
export import tree;
export import hash;

namespace myvc {

using std::unique_ptr, std::shared_ptr, std::make_shared, std::logic_error, std::invalid_argument, std::string, std::vector, std::ostream, std::istream, std::getline;
using namespace std::chrono;
using datetime = time_point<system_clock>;

export class Commit : public Object {
    Hash treeHash;
    vector<Hash> parentHashes;
    datetime time;
    string msg;

    void write(ostream &out) const noexcept override {
        out << "Commit" << '\n' << treeHash << '\n' << parentHashes.size() << '\n';
        for(const auto &h : parentHashes) out << h << '\n';
        out << system_clock::to_time_t(time) << '\n' << msg << '\n';
    }

public:
    explicit Commit(Hash treeHash, vector<Hash> parentHashes = {}, datetime time = system_clock::now(), string msg = "") :
        treeHash {std::move(treeHash)}, parentHashes {std::move(parentHashes)}, time {std::move(time)}, msg {std::move(msg)}
    {
        if(msg.find('\n') != string::npos) throw invalid_argument {"Commit::Commit() called with newline in msg"};
    }

    static Commit read(istream &in) {
        string s; Hash h, k; size_t n; vector<Hash> v; time_t t;
        in >> s;
        if(s == "Commit") {
            in >> h >> n;
            for(size_t i = 0; i < n; ++i) {
                in >> k;
                v.push_back(k);
            }
            in >> t;
            getline(in, s);
            return Commit {h, v, system_clock::from_time_t(t), s};
        }
        throw invalid_argument {"Commit::read() failed parsing"};
    }

    Hash getHash() const override {
        return Hash {"temporary_hash"};
    }

    bool isRoot() const {
        return parentHashes.empty();
    }

    const Hash &getParentHash() const {
        if(parentHashes.size() != 1) throw logic_error {"Commit::getParent() called on node with != 1 parent"};
        return parentHashes[0];
    }

    const vector<Hash> &getParentHashes() const {
        return parentHashes;
    }

    const datetime &getTime() const {
        return time;
    }

    const string &getMsg() const {
        return msg;
    }

    const Hash &getTreeHash() const {
        return treeHash;
    }
};

}
