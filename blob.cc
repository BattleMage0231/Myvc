#include <sstream>
#include "blob.h"
#include "serialize.h"
#include "errors.h"
#include "debug.h"

using namespace myvc;

static std::vector<std::string> getLines(const std::vector<char> &data) {
    std::stringstream ss {{ data.begin(), data.end() }};
    std::vector<std::string> res;
    std::string line;
    while(std::getline(ss, line)) {
        res.emplace_back(line);
    }
    if(!data.empty() && data.back() == '\n') res.emplace_back("");
    return res;
}

Diff Blob::diff(const Blob &a, const Blob &b) {
    auto aLines = getLines(a.getData()), bLines = getLines(b.getData());
    return Diff {aLines, bLines};
}

Blob::Blob(std::vector<char> data) 
    : data {std::move(data)} {}

Blob::Blob(const std::vector<std::string> &lines) {
    for(size_t i = 0; i < lines.size(); ++i) {
        if(i != 0) data.emplace_back('\n');
        data.insert(data.end(), lines[i].begin(), lines[i].end());
    }
}

void Blob::write(std::ostream &out) const {
    write_raw(out, data.size());
    out.write(data.data(), data.size());
}

void Blob::read(std::istream &in) {
    size_t sz;
    read_raw(in, sz);
    data.resize(sz);
    in.read(data.data(), sz);
}

const std::vector<char> &Blob::getData() const {
    return data;
}

bool Blob::operator==(const Blob &other) const {
    return data == other.data;
}
