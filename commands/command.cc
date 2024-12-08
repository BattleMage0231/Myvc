#include <sstream>
#include "command.h"
#include "../head.h"
#include "../store.h"

using namespace myvc;
using namespace myvc::commands;

Command::Command(const fs::path &base, std::vector<std::string> rawArgs, bool useRepo)
    : basePath {fs::canonical(base)}, rawArgs {std::move(rawArgs)}, useRepo {useRepo}
{
    if(!useRepo) {
        repoPath = basePath;
        return;
    }
    for(fs::path p = basePath; p != "/"; p = p.parent_path()) {
        if(RepositoryStore::existsAt(p)) {
            repoPath = p;
            return;
        }
    }
    throw command_error {"no repository found"};
}

void Command::createRules() {
    addFlagRule("--help");
    addFlagRule("--h");
}

void Command::addFlagRule(std::string flag, size_t num) {
    flagRules.insert_or_assign(std::move(flag), num);
}

bool Command::hasFlag(const std::string &flag) const {
    return flagArgs.find(flag) != flagArgs.end();
}

const std::vector<std::string> &Command::getFlagArgs(const std::string &flag) const {
    return flagArgs.at(flag);
}

size_t Command::resolveNumber(const std::string &str) const {
    try {
        return std::stoull(str);
    } catch(...) {
        throw command_error {"illegal number " + str};
    }
}

Commit Command::resolveSymbol(const std::string &str) const {
    std::stringstream ss {str};
    std::string token;
    std::getline(ss, token, '^');
    Commit c;
    if(token == "HEAD") {
        Head &head = repo->getHead();
        if(head.hasState()) {
            c = head.getCommit();
        } else {
            throw command_error {"HEAD does not exist"};
        }
    } else {
        auto maybeBranch = repo->getBranch(token);
        if(maybeBranch) {
            c = maybeBranch.value().get().getCommit();
        } else {
            auto maybeHash = repo->resolvePartialHash(token);
            if(maybeHash) {
                // assume object is commit
                c = repo->getCommit(maybeHash.value()).value();
            } else {
                throw command_error {"invalid hash " + token};
            }
        }
    }
    while(std::getline(ss, token, '^')) {
        size_t n = token == "" ? 0 : (resolveNumber(token) - 1);
        c = c.getParents().at(n);
    }
    return c;
}

Branch &Command::resolveBranch(const std::string &name) const {
    auto maybeBranch = repo->getBranch(name);
    if(maybeBranch) return maybeBranch.value();
    throw command_error {"no branch with name " + name};
}

fs::path Command::resolvePath(const std::string &p) const {
    try {
        fs::path res {p};
        if(!res.is_absolute()) res = basePath / res;
        res = res.lexically_normal().lexically_relative(repoPath);
        if(res.empty()) {
            throw command_error {"path " + p + " not in repository at " + static_cast<std::string>(repoPath)};
        }
        if(!res.lexically_relative(RepositoryStore::myvcName).empty()) {
            throw command_error {"path " + p + " must not point to the myvc internal directory"};
        }
        return res;
    } catch(const command_error &e) {
        throw e;
    } catch(...) {
        throw command_error {"malformed path " + p};
    }
}

void Command::execute() {
    createRules();
    // parse arguments
    for(size_t i = 0; i < rawArgs.size();) {
        std::string cur = rawArgs.at(i++);
        if(flagRules.find(cur) != flagRules.end()) {
            size_t needed = flagRules.at(cur);
            if(rawArgs.size() < i + needed) {
                throw command_error {"flag " + cur + " requires " + std::to_string(needed) + " arguments"};
            }
            std::vector<std::string> v;
            for(; needed > 0; --needed, ++i) {
                v.emplace_back(rawArgs[i]);
            }
            flagArgs.insert_or_assign(cur, std::move(v));
        } else {
            args.emplace_back(cur);
        }
    }
    if(hasFlag("-h") || hasFlag("--help")) {
        printHelpMessage();
    } else {
        if(useRepo) repo = std::make_shared<Repository>(repoPath);
        fs::path currentPath = fs::current_path();
        fs::current_path(repoPath);
        try {
            process();
        } catch(...) {
            fs::current_path(currentPath);
            throw;
        }
    }
}

void Command::expectNumberOfArgs(size_t number) const {
    if(args.size() != number) {
        throw command_error {"invalid number of arguments"};
    }
}

Command::~Command() {}
