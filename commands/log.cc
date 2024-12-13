#include <algorithm>
#include <iostream>
#include <vector>
#include <chrono>
#include <format>
#include <sstream>
#include "log.h"

using myvc::Commit;
using namespace myvc::commands;

Log::Log(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Log::printHelpMessage() const {
    std::cerr << "usage: myvc log [-n NUM] [--pretty] [commit]" << std::endl;
    std::cerr << "Displays reachable commits from a given commit." << std::endl;
    std::cerr << "log without commit displays reachable commits from HEAD." << std::endl;
    std::cerr << "log commit displays reachable commits from a given commit." << std::endl;
    std::cerr << "log -n NUM commit displays the NUM most recent rechable commits." << std::endl;
    std::cerr << "log --pretty commit displays the reachable commits in a tree-like format." << std::endl;
}

void Log::createRules() {
    Command::createRules();
    addFlagRule("-n", 1);
    addFlagRule("--pretty", 0);
}

static std::string pad(size_t n) {
    std::stringstream ss;
    for(size_t i = 0; i < n; ++i) ss << ' ';
    return ss.str();
}

static void printCommit(const Commit &c, size_t depth = 0) {
    std::cout << pad(depth) << "commit " << c.hash() << std::endl;
    auto timePoint = std::chrono::system_clock::from_time_t(c.getTime());
    auto time = std::chrono::current_zone()->to_local(timePoint);
    std::cout << pad(depth) << "Date:    " << std::format("{:%a %b %d %H:%M:%S %Y}", time) << std::endl;
    std::cout << pad(depth) << "Message: " << c.getMsg() << std::endl;
}

static void prettyprint(const Commit &c, size_t depth = 0) {
    printCommit(c, depth);
    auto parents = c.getParents();
    if(!parents.empty()) {
        std::cout << pad(depth + 1) << '|' << std::endl;
        for(const Commit &par : parents) {
            prettyprint(par, depth + 1);
        }
    }
}

void Log::process() {
    Commit c;
    if(args.size() == 0) {
        Head &head = repo->getHead();
        if(head.hasState()) {
            c = head.getCommit();
        } else {
            throw command_error {"HEAD does not exist"};
        }
    } else {
        expectNumberOfArgs(1);
        c = resolveSymbol(args.at(0));
    }
    if(hasFlag("--pretty")) {
        prettyprint(c);
        return;
    }
    auto commitSet = Commit::getAllReachable(c);
    std::vector<Commit> commits {commitSet.begin(), commitSet.end()};
    std::sort(commits.begin(), commits.end(), [](const Commit &a, const Commit &b) {
        return a.getTime() > b.getTime();
    });
    if(hasFlag("-n")) {
        size_t n = resolveNumber(getFlagArgs("-n").at(0));
        if(commits.size() > n) commits.resize(n);
    }
    for(const Commit &c : commits) {
        printCommit(c);
        std::cout << std::endl;
    }
}
