#include "command.h"

using namespace myvc;
using namespace myvc::commands;

Command::Command(fs::path repoPath, std::vector<std::string> rawArgs, bool useStore)
    : rawArgs {std::move(rawArgs)}, useStore {useStore}, repoPath {fs::canonical(repoPath)} {}

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
                store = std::make_shared<RepositoryStore>(repoPath);
            } catch(...) {
                throw command_error {"repository does not exist"};
            }
        }
        process();
    }
}

size_t Command::resolveNumber(const std::string &str) const {
    try {
        return std::stoull(str);
    } catch(...) {
        throw command_error {"illegal number " + str};
    }
}

Commit Command::resolveSymbol(const std::string &str) const {
    // extract commit part
    size_t split = str.find('^');
    std::string main = str.substr(0, split);
    try {
        Commit c = store->getCommit(store->resolvePartialObjectHash(main).value()).value();
        return c;
    } catch(...) {
        throw command_error {"illegal commit symbol " + str};
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

fs::path Command::resolvePath(const std::string &p) const {
    fs::path res;
    try {
        res = fs::weakly_canonical(fs::absolute(fs::path {p}));
    } catch(...) {
        throw command_error {"malformed path " + p};
    }
    ensureWithinRepo(res);
    return res;
}

fs::path Command::getRelative(const fs::path &abs) const {
    return fs::proximate(abs, repoPath);
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
    if(std::mismatch(p.begin(), p.end(), repoPath.begin(), repoPath.end()).second != repoPath.end()) {
        throw command_error {"path " + static_cast<std::string>(p) + " is not within the repository at " + static_cast<std::string>(repoPath)};
    }
}
