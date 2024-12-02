#include "command.h"

using namespace myvc;
using namespace myvc::commands;

Command::Command(fs::path path, std::vector<std::string> rawArgs, bool useStore)
    : rawArgs {std::move(rawArgs)}, useStore {useStore}, path {fs::canonical(path)} {}

void Command::parseArgs() {
    for(size_t i = 0; i < rawArgs.size();) {
        std::string cur = rawArgs[i++];
        if(flagRules.find(cur) != flagRules.end()) {
            size_t needed = flagRules[cur];
            flagArgs[cur];
            if(rawArgs.size() - i < needed) throw std::runtime_error {""};
            for(; needed > 0; --needed, ++i) {
                flagArgs[cur].emplace_back(rawArgs[i]);
            }
        } else {
            args.push_back(cur);
        }
    }
}

void Command::execute() {
    flagRules["--help"] = 0;
    flagRules["-h"] = 0;
    createRules();
    parseArgs();
    if(flagArgs.find("-h") != flagArgs.end() || flagArgs.find("--help") != flagArgs.end()) {
        printHelpMessage();
    } else {
        if(useStore) {
            try {
                store = std::make_shared<RepositoryStore>(path);
            } catch(...) {
                throw command_error {"repository does not exist"};
            }
        }
        process();
    }
}

std::vector<char> hexToChars(const std::string &str) {
    std::stringstream ss {str};
    std::vector<char> res;
    for(size_t i = 0; i < str.size(); ++i) {
        std::string b = str.substr(i, 2);
        res.push_back(static_cast<char>(std::stoi(b, nullptr, 16)));
    }
    return res;
}

size_t Command::resolveNumber(const std::string &str) const {
    try {
        return std::stoull(str);
    } catch(...) {
        throw command_error {"illegal number " + str};
    }
}

Commit Command::resolveSymbol(const std::string &str) const {
    if(str == "HEAD") {
        return *resolveHead();
    } else {
        // resolve commit
        std::vector<char> chars;
        try {
            chars = hexToChars(str);
        } catch(...) {
            throw command_error {"illegal commit symbol " + str};
        }
        if(chars.size() != 20) throw command_error {"illegal commit symbol " + str};
        Hash h { hexToChars(str) };
        try {
            return store->getCommit(h).value();
        } catch(...) {
            throw command_error {"commit " + str + " does not exist"}; 
        }
    }
}

Head Command::resolveHead() const {
    auto head = store->getHead();
    if(head) return *head;
    else throw command_error {"HEAD does not exist"}; 
}

Index Command::resolveIndex() {
    auto index = store->getIndex();
    if(index) return *index;
    else {
        Tree t {{}, store};
        t.store();
        Index index {t.getHash(), t.getHash(), store};
        index.store();
        return index;
    }
}

fs::path Command::resolvePath(const std::string &p, bool mustExist) const {
    fs::path res;
    try {
        res = fs::path {p};
    } catch(...) {
        throw command_error {"malformed path " + p};
    }
    if(mustExist) ensureExists(res);
    ensureWithinRepo(res);
    return fs::weakly_canonical(res);
}

fs::path Command::getRelative(const fs::path &abs) const {
    fs::path rel;
    auto it = abs.begin();
    for(auto baseIt = path.begin(); baseIt != path.end(); ++baseIt) ++it;
    for(; it != abs.end(); ++it) rel /= *it;
    return rel;
}

void Command::ensureIsFile(const fs::path &p) const {
    if(fs::is_directory(p)) {
        throw command_error {"path " + static_cast<std::string>(p) + " is not a file"};
    }
}

void Command::ensureExists(const fs::path &p) const {
    if(!fs::exists(p)) {
        throw command_error {"path " + static_cast<std::string>(p) + " does not exist"};
    }
}

void Command::ensureWithinRepo(const fs::path &p) const {
    if(std::mismatch(p.begin(), p.end(), path.begin(), path.end()).second != path.end()) {
        throw command_error {"path " + static_cast<std::string>(p) + " is not within the repository at " + static_cast<std::string>(path)};
    }
}
