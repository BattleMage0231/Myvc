#include <algorithm>
#include <utility>
#include "diff.h"
#include "errors.h"

using namespace myvc;

// this implementation of (an unoptimized version) of the Myers diff algorithm is based on the discussion here:
// https://blog.jcoglan.com/2017/02/15/the-myers-diff-algorithm-part-2/

std::vector<Hunk> diff(const std::vector<std::string> &base, const std::vector<std::string> &other) {
    size_t n = base.size(), m = other.size();
    size_t d[n + 1][m + 1];
    for(size_t i = 0; i <= n; ++i) d[i][0] = i;
    for(size_t i = 0; i <= m; ++i) d[0][i] = i;
    for(size_t i = 1; i <= n; ++i) {
        for(size_t j = 1; j <= m; ++j) {
            if(base[i - 1] == other[j - 1]) {
                d[i][j] = d[i - 1][j - 1];
            } else {
                d[i][j] = 1 + std::min(d[i - 1][j], d[i][j - 1]);
            }
        }
    }
    std::vector<std::pair<size_t, Change>> changes;
    size_t i = n, j = m;
    while(i > 0 && j > 0) {
        if(base[i - 1] == other[j - 1]) {
            --i; --j;
        } else if(d[i - 1][j] < d[i][j - 1]) {
            --i;
            changes.emplace_back(std::make_pair(i, Change {Change::Type::Delete, base[i]}));
        } else {
            changes.emplace_back(std::make_pair(i, Change {Change::Type::Add, other[--j]}));
        }
    }
    while(i > 0) {
        --i;
        changes.emplace_back(std::make_pair(i, Change {Change::Type::Delete, base[i]}));
    }
    while(j > 0) {
        changes.emplace_back(std::make_pair(0, Change {Change::Type::Add, other[--j]}));
    }
    std::vector<Hunk> hunks;
    for(auto it = changes.rbegin(); it != changes.rend();) {
        std::vector<Change> v = {it->second};
        size_t oldLine = it->first;
        size_t line = oldLine + ((it++)->second.type == Change::Type::Delete);
        for(; it != changes.rend() && it->first == line; ++it) {
            v.emplace_back(it->second);
            if(it->second.type == Change::Type::Delete) ++line;
        }
        hunks.emplace_back(oldLine, std::move(v));
    }
    if(hunks.size() == 0) return hunks;
    if(hunks.front().getIndex() != 0) hunks.insert(hunks.begin(), Hunk {0});
    if(hunks.back().getEnd() != n) hunks.insert(hunks.end(), Hunk {n});
    return hunks;
}

Hunk::Hunk(size_t index, std::vector<Change> changes) 
    : index {index}, end {index}, changes {std::move(changes)} 
{
    for(const auto &change : this->changes) {
        if(change.type == Change::Type::Delete) ++end;
    }
}

size_t Hunk::getIndex() const {
    return index;
}

size_t Hunk::getEnd() const {
    return end;
}

const std::vector<Change> &Hunk::getChanges() const {
    return changes;
}

std::vector<std::string> Hunk::getOurs() const {
    std::vector<std::string> res;
    for(const auto &change : this->changes) {
        if(change.type == Change::Type::Delete) res.emplace_back(change.content);
    }
    return res;
}

std::vector<std::string> Hunk::getTheirs() const {
    std::vector<std::string> res;
    for(const auto &change : this->changes) {
        if(change.type == Change::Type::Add) res.emplace_back(change.content);
    }
    return res;
}

std::pair<Diff, Diff::Conflicts> Diff::merge(const Diff &a, const Diff &b) {
    std::vector<std::string> base = a.getBase();
    auto aHunks = a.getHunks(), bHunks = b.getHunks();
    auto aIt = aHunks.begin(), bIt = bHunks.begin();
    std::vector<Hunk> hunks;
    std::vector<std::pair<Hunk, Hunk>> conflicts;
    while(aIt != aHunks.end() && bIt != bHunks.end()) {
        if(*aIt == *bIt) {
            hunks.emplace_back(*aIt);
            ++aIt; ++bIt;
        } else {
            size_t as = aIt->getIndex(), ae = aIt->getEnd();
            size_t bs = bIt->getIndex(), be = bIt->getEnd();
            if((as <= bs && bs <= ae) || (bs <= as && as <= be)) {
                if(aIt->getChanges().empty()) {
                    hunks.emplace_back(*bIt);
                    ++aIt; ++bIt;
                } else if(bIt->getChanges().empty()) {
                    hunks.emplace_back(*aIt);
                    ++aIt; ++bIt;
                } else {
                    size_t start = std::min(as, bs), end;
                    std::vector<Change> aChanges, bChanges;
                    auto aStart = aIt, bStart = bIt;
                    while(aIt != aHunks.end() && bIt != bHunks.end()) {
                        as = aIt->getIndex(); ae = aIt->getEnd();
                        bs = bIt->getIndex(); be = bIt->getEnd();
                        if((as <= bs && bs <= ae) || (bs <= as && as <= be)) {
                            end = std::max(ae, be);
                            if(ae == be) {
                                ++aIt; ++bIt;
                                break;
                            }
                            if(ae < be) ++aIt;
                            else ++bIt;
                        } else {
                            end = std::min(ae, be);
                            if(ae > be) ++aIt;
                            else ++bIt;
                            break;
                        }
                    }
                    for(size_t i = start; i < end; ++i) {
                        aChanges.emplace_back(Change { Change::Type::Delete, base[i] });
                        bChanges.emplace_back(Change { Change::Type::Delete, base[i] });
                    }
                    for(size_t i = start; aStart != aIt; i = aStart->getEnd(), ++aStart) {
                        while(i < aStart->getIndex()) {
                            aChanges.emplace_back(Change { Change::Type::Add, base[i++] });
                        }
                        for(const auto &change : aStart->getChanges()) {
                            if(change.type == Change::Type::Add) aChanges.emplace_back(change);
                        }
                    }
                    for(size_t i = start; bStart != bIt; i = bStart->getEnd(), ++bStart) {
                        while(i < bStart->getIndex()) {
                            bChanges.emplace_back(Change { Change::Type::Add, base[i++] });
                        }
                        for(const auto &change : bStart->getChanges()) {
                            if(change.type == Change::Type::Add) bChanges.emplace_back(change);
                        }
                    }
                    for(size_t i = (--aStart)->getEnd(); i < end; ++i) aChanges.emplace_back(Change { Change::Type::Add, base[i] });
                    for(size_t i = (--bStart)->getEnd(); i < end; ++i) bChanges.emplace_back(Change { Change::Type::Add, base[i] });
                    conflicts.emplace_back(std::make_pair(Hunk { start, std::move(aChanges) }, Hunk { start, std::move(bChanges) }));
                }
            } else if(as <= bs) {
                hunks.emplace_back(*aIt);
                ++aIt;
            } else {
                hunks.emplace_back(*bIt);
                ++bIt;
            }
        }
    }
    hunks.insert(hunks.end(), aIt, aHunks.end());
    hunks.insert(hunks.end(), bIt, bHunks.end());
    return { Diff {a.getBase(), std::move(hunks)}, conflicts };
}

Diff::Diff(std::vector<std::string> base, const std::vector<std::string> &other) 
    : base {std::move(base)}, hunks {std::move(diff(this->base, other))} {}

Diff::Diff(std::vector<std::string> base, std::vector<Hunk> hunks) 
    : base {std::move(base)}, hunks {std::move(hunks)} {}

std::vector<std::string> Diff::apply() const {
    if(hunks.empty()) return base;
    std::vector<std::string> res = hunks.front().getTheirs();
    for(size_t i = 1; i < hunks.size(); ++i) {
        res.insert(res.end(), base.begin() + hunks[i - 1].getEnd(), base.begin() + hunks[i].getIndex());
        auto hunkRes = hunks[i].getTheirs();
        res.insert(res.end(), hunkRes.begin(), hunkRes.end());
    }
    return res;
}

const std::vector<std::string> &Diff::getBase() const {
    return base;
}

const std::vector<Hunk> &Diff::getHunks() const {
    return hunks;
}
