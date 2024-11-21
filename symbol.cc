export module symbol;

export import <string>;

export import commit;

namespace myvc {

using std::string;

export class Symbol {
    string name;

public:
    Symbol(const string &name) : name {name} {}

    virtual ~Symbol() {}

    const string &getName() const {
        return name;
    }

    virtual Commit &operator*() = 0;
    virtual const Commit &operator*() const = 0;
};

}
