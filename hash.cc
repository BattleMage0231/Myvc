module;

#include <string>
#include <iostream>
#include <compare>
#include <stdexcept>

export module hash;

namespace myvc {

using std::string;

struct HashTemp {
    string s;

public:
    HashTemp() {}

    HashTemp(string s) : s {std::move(s)} {
        for(char c : s) {
            if(c == ' ' || c == '\n') throw std::invalid_argument {"HashTemp::HashTemp() called with invalid string"};
        }
    }

    auto operator<=>(const HashTemp &other) const noexcept {
        return s <=> other.s;
    }

    bool operator==(const HashTemp &) const noexcept = default;

    friend std::istream &operator>>(std::istream &in, HashTemp &tmp) {
        in >> tmp.s;
        return in;
    }

    friend std::ostream &operator<<(std::ostream &out, const HashTemp &tmp) noexcept {
        out << tmp.s;
        return out;
    }

    operator string() const {
        return s;
    }
};

export using Hash = HashTemp;

}
