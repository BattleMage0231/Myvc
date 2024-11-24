module;

#include <iostream>
#include <stdexcept>
#include <string>

export module blob;

import hash;
import object;

namespace myvc {

using std::string;

export class Blob : public Object {
    string data; // todo use vector<unsigned char> instead

    void write(std::ostream &out) const noexcept override {
        out << "Blob" << '\n';
        out << data;
    }
public:
    Blob(string data) : data {data} {}

    static Blob read(std::istream &in) {
        // todo make this better
        string s; char c;
        in >> s;
        if(s == "Blob") {
            in >> c >> s;
            return Blob {s};
        }
        throw std::invalid_argument {"Commit::read() failed parsing"};
    }

    Hash getHash() const noexcept {
        return Hash {"temp_hash"};
    }

    const string &getData() const {
        return data;
    }
};

}
