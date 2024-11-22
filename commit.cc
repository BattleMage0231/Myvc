export module commit;

export import <memory>;
export import <chrono>;
import <stdexcept>;
import <vector>;

export import object;
export import tree;
export import hash;

namespace myvc {

using std::unique_ptr, std::shared_ptr, std::make_shared, std::logic_error, std::invalid_argument, std::string, std::vector, std::ostream, std::istream;
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
    explicit Commit(const Hash &treeHash, const vector<Hash> &parentHashes = {}, const datetime &time = system_clock::now(), const string &msg = "") :
        treeHash {treeHash}, parentHashes {parentHashes}, time {time}, msg {msg}
    {
        if(msg.find('\n') != string::npos) throw invalid_argument {"Commit::Commit() called with newline in msg"};
    }

    static unique_ptr<Commit> read(istream &in) {
        string s; Hash h, k; size_t n; vector<Hash> v; time_t t;
        in >> s;
        if(s == "Commit") {
            in >> h >> n;
            for(size_t i = 0; i < n; ++i) {
                in >> k;
                v.push_back(k);
            }
            in >> t >> s;
            return make_unique<Commit>(h, v, system_clock::from_time_t(t), s);
        }
        throw invalid_argument {"Commit::read() failed parsing"};
    }

    const Hash &getHash() const override {
        return "TEMP HASH";
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
