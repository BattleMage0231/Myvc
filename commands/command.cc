#include <sstream>
#include "command.h"
#include "../head.h"
#include "../store.h"
#include "../debug.h"

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

Commit Command::resolvePureSymbol(const std::string &token) const {
    if(token == "HEAD") {
        Head &head = repo->getHead();
        if(head.hasState()) {
            return head.getCommit();
        } else {
            throw command_error {"HEAD does not exist"};
        }
    } else {
        auto maybeBranch = repo->getBranch(token);
        if(maybeBranch) {
            return maybeBranch.value().get().getCommit();
        } else {
            try {
                return repo->getCommit(resolveHash(token)).value();
            } catch(...) {
                throw command_error {"invalid commit " + token};
            }
        }
    }
}

Commit Command::resolveSymbol(const std::string &str) const {
    std::vector<std::string> split;
    std::stringstream ss {str};
    std::string tok;
    while(std::getline(ss, tok, '^')) {
        split.emplace_back(std::move(tok));
    }
    if(str.back() == '^') split.emplace_back("");
    Commit c = resolvePureSymbol(split.at(0));
    for(size_t i = 1; i < split.size(); ++i) {
        size_t n = split.at(i) == "" ? 0 : resolveNumber(split.at(i));
        auto parents = c.getParents();
        if(n >= c.getParents().size()) {
            throw command_error {"invalid commit reference " + str};
        }
        c = parents.at(n);
    }
    myvc::debug_log("resolved " + str + " to " + static_cast<std::string>(c.hash()));
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
        if(res.empty() || *res.begin() == "..") {
            throw command_error {"path " + p + " not in repository at " + static_cast<std::string>(repoPath)};
        }
        fs::path rel = res.lexically_relative(RepositoryStore::myvcName);
        if(!rel.empty() && *rel.begin() != "..") {
            throw command_error {"path " + p + " must not point to the myvc internal directory"};
        }
        myvc::debug_log("resolve path " + p + " to " + static_cast<std::string>(res));
        return res;
    } catch(const command_error &e) {
        throw e;
    } catch(...) {
        throw command_error {"malformed path " + p};
    }
}

Hash Command::resolveHash(const std::string &s) const {
    auto maybeHash = repo->resolvePartialHash(s);
    if(maybeHash) return maybeHash.value();
    else throw command_error {"nonexistent hash " + s};
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

void Command::expectExists(const fs::path &path) const {
    Tree workingTree = repo->getWorkingTree();
    if(!workingTree.getAtPath(path).has_value()) {
        throw command_error {"path " + static_cast<std::string>(path) + " does not exist in the working directory"};
    }
}

void Command::expectIsDir(const fs::path &p) const {
    Tree workingTree = repo->getWorkingTree();
    auto thing = workingTree.getAtPath(p);
    if(!thing.has_value() || !std::holds_alternative<Tree>(thing.value())) {
        throw command_error {"directory " + static_cast<std::string>(p) + " does not exist in the working directory"};
    }
}

void Command::expectIsFile(const fs::path &p) const {
    Tree workingTree = repo->getWorkingTree();
    auto thing = workingTree.getAtPath(p);
    if(!thing.has_value() || !std::holds_alternative<Blob>(thing.value())) {
        throw command_error {"file " + static_cast<std::string>(p) + " does not exist in the working directory"};
    }
}

void Command::expectCleanState() const {
    Index &index = repo->getIndex();
    Head &head = repo->getHead();
    if(!head.hasState()) {
        throw command_error {"nonexistent head"};
    }
    TreeDiff diff1 = Tree::diff(repo->getWorkingTree(), index.getTree());
    TreeDiff diff2 = Tree::diff(index.getTree(), head.getCommit().getTree());
    if(!diff1.getChanges().empty() || !diff2.getChanges().empty()) {
        throw command_error {"clean working directory and index expected"};
    }
}

Command::~Command() {}
