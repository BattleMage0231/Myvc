#include <sstream>
#include <format>
#include <vector>
#include <algorithm>
#include <cassert>
#include <bit>
#include "hash.h"
#include "serialize.h"

using namespace myvc;

// this implementation of the SHA1 hashing algorithm is based on the pseudocode here:
// https://en.wikipedia.org/wiki/SHA-1#Examples_and_pseudocode

struct State {
    uint32_t h[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
};

std::vector<uint32_t> preprocess(const std::vector<char> &input) {
    std::vector<uint8_t> res(input.size());
    std::transform(input.begin(), input.end(), res.begin(), [](char c) { return static_cast<uint8_t>(c); });
    // append 1 bit
    res.emplace_back(0x80);
    // append 0 bits until length = 56 (mod 64)
    size_t padding = (120 - res.size() % 64) % 64;
    res.resize(res.size() + padding, 0x00);
    // append the length in big endian
    uint64_t size = 8 * static_cast<uint64_t>(input.size());
    for(size_t i = 0; i < 8; ++i) {
        res.emplace_back(static_cast<uint8_t>((size >> (8 * (7 - i)))));
    }
    assert(res.size() % 64 == 0);
    // convert to 32-bit blocks
    std::vector<uint32_t> res2(res.size() / 4, 0);
    for(size_t i = 0; i < res.size() / 4; ++i) {
        for(size_t j = 0; j < 4; ++j) {
            res2[i] = (res2[i] << 8) | res[4 * i + j];
        }
    }
    return res2;
}

void process_chunk(std::vector<uint32_t> &h, std::vector<uint32_t> chunk) {
    assert(chunk.size() == 16);
    // extend chunk
    chunk.resize(80);
    for(size_t i = 16; i < 80; ++i) {
        chunk[i] = std::rotl(chunk[i - 3] ^ chunk[i - 8] ^ chunk[i - 14] ^ chunk[i - 16], 1);
    }
    // main loop
    std::vector<uint32_t> a = h;
    for(size_t i = 0; i < 80; ++i) {
        uint32_t f, k;
        if(i < 20) {
            f = (a[1] & a[2]) | ((~a[1]) & a[3]);
            k = 0x5A827999;
        } else if(i < 40) {
            f = a[1] ^ a[2] ^ a[3];
            k = 0x6ED9EBA1;
        } else if(i < 60) {
            f = (a[1] & a[2]) | (a[1] & a[3]) | (a[2] & a[3]);
            k = 0x8F1BBCDC;
        } else {
            f = a[1] ^ a[2] ^ a[3];
            k = 0xCA62C1D6;
        }
        uint32_t tmp = std::rotl(a[0], 5) + f + a[4] + k + chunk[i];
        a[4] = a[3];
        a[3] = a[2];
        a[2] = std::rotl(a[1], 30);
        a[1] = a[0];
        a[0] = tmp;
    }
    // add results to state
    for(size_t i = 0; i < 5; ++i) h[i] += a[i];
}

std::vector<char> hash(const std::vector<char> &input) {
    std::vector<uint32_t> src = preprocess(input);
    assert(src.size() % 16 == 0);
    // process all chunks
    std::vector<uint32_t> h { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
    for(size_t i = 0; i < src.size(); i += 16) {
        std::vector<uint32_t> chunk(16);
        std::copy(src.begin() + i, src.begin() + i + 16, chunk.begin());
        process_chunk(h, std::move(chunk));
    }
    // build final hash
    std::vector<char> res(20, 0);
    for(size_t i = 0; i < 5; ++i) {
        uint32_t word = h[i];
        for(size_t j = 0; j < 4; ++j) {
            uint8_t byte = static_cast<uint8_t>(word >> (8 * (3 - j)));
            res[4 * i + j] = static_cast<char>(byte);
        }
    }
    assert(res.size() == 20);
    return res;
}

SHA1Hash::SHA1Hash() : bytes {} {}

SHA1Hash::SHA1Hash(const Serializable &s) {
    std::stringstream ss;
    s.write(ss);
    std::string str = ss.str();
    std::vector<char> data {str.begin(), str.end()};
    std::vector<char> hashed = hash(data);
    for(size_t i = 0; i < 20; ++i) bytes[i] = hashed[i];
}

SHA1Hash::SHA1Hash(const std::vector<char> &b) {
    assert(b.size() == 20);
    for(size_t i = 0; i < 20; ++i) bytes[i] = b[i];
}

std::strong_ordering SHA1Hash::operator<=>(const SHA1Hash &other) const {
    for(size_t i = 0; i < 20; ++i) {
        if(bytes[i] != other.bytes[i]) return bytes[i] <=> other.bytes[i];
    }
    return std::strong_ordering::equivalent;
}

bool SHA1Hash::operator==(const SHA1Hash &other) const {
    return (*this <=> other) == 0;
}

void SHA1Hash::write(std::ostream &out) const {
    out.write(bytes, sizeof(bytes));
}

void SHA1Hash::read(std::istream &in) {
    in.read(bytes, 20 * sizeof(char));
}

SHA1Hash::operator std::string() const {
    std::stringstream ss;
    for(char c : bytes) {
        ss << std::format("{:02x}", static_cast<unsigned char>(c));
    }
    return ss.str();
}
