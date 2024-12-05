#include <sstream>
#include "blob.h"
#include "serialize.h"
#include "errors.h"

using namespace myvc;

std::vector<std::string> getLines(const std::vector<char> &data) {
    std::stringstream ss {{ data.begin(), data.end() }};
    std::vector<std::string> res;
    std::string line;
    while(std::getline(ss, line)) {
        res.emplace_back(line);
    }
    return res;
}

Diff Blob::diff(const Blob &a, const Blob &b) {
    auto aLines = getLines(a.getData()), bLines = getLines(b.getData());
    return Diff {aLines, bLines};
}

Blob::Blob(std::vector<char> data, std::shared_ptr<Provider> prov) 
    : data {std::move(data)}, prov {std::move(prov)} {}

Blob::Blob(std::istream &in, std::shared_ptr<Provider> prov) : prov {std::move(prov)} {
    read(in);
}

Blob::Blob(const std::vector<std::string> &lines, std::shared_ptr<Provider> prov) : prov {std::move(prov)} {
    for(size_t i = 0; i < lines.size(); ++i) {
        if(i != 0) data.push_back('\n');
        data.insert(data.end(), lines[i].begin(), lines[i].end());
    }
}

void Blob::write(std::ostream &out) const {
    write_raw(out, data.size());
    for(char c : data) write_raw(out, c);
}

void Blob::read(std::istream &in) {
    size_t sz;
    read_raw(in, sz);
    data.clear();
    for(size_t i = 0; i < sz; ++i) {
        char c;
        read_raw(in, c);
        data.emplace_back(c);
    }
}

void Blob::store() {
    prov->createBlob(*this);
}

std::vector<char> &Blob::getData() {
    return data;
}

const std::vector<char> &Blob::getData() const {
    return data;
}

void Blob::setProvider(std::shared_ptr<Provider> prov) {
    this->prov = std::move(prov);
}

