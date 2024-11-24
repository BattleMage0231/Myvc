module;

#include <string>
#include <iostream>
#include <compare>
#include <stdexcept>

export module hash;

namespace myvc {

using std::istream, std::ostream, std::string, std::invalid_argument;

struct HashTemp {
    string s;

public:
    HashTemp() {}

    HashTemp(string s) : s {std::move(s)} {
        for(char c : s) {
            if(c == ' ' || c == '\n') throw invalid_argument {"HashTemp::HashTemp() called with invalid string"};
        }
    }

    std::strong_ordering operator<=>(const HashTemp &other) const {
        return s <=> other.s;
    }

    friend istream &operator>>(istream &in, HashTemp &tmp) {
        in >> tmp.s;
        return in;
    }

    friend ostream &operator<<(ostream &out, const HashTemp &tmp) {
        out << tmp.s;
        return out;
    }

    operator string() const {
        return s;
    }
};

export using Hash = HashTemp;

}
