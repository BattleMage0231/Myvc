#include "command.h"

using namespace myvc;
using namespace myvc::commands;

Command::Command(fs::path path, std::vector<std::string> rawArgs, bool useStore)
    : rawArgs {std::move(rawArgs)}, useStore {useStore}, path {std::move(path)} {}

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