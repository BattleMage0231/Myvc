#include <algorithm>
#include <iostream>
#include <vector>
#include "log.h"
#include "../hash.h"

using namespace myvc::commands;

Log::Log(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Log::printHelpMessage() {
    std::cerr << "usage: myvc log [-n NUM] commit" << std::endl;
}

void Log::createRules() {
    Command::createRules();
    flagRules["-n"] = 1;
}

void Log::process() {
    Commit c = (args.size() == 0) ? *resolveHead() : resolveSymbol(args[0]);
    std::vector<Commit> commits = Commit::getAllReachable(c);
    std::sort(commits.begin(), commits.end(), [](const Commit &a, const Commit &b) {
        return a.getTime() > b.getTime();
    });
    if(flagArgs.find("-n") != flagArgs.end()) {
        size_t n = resolveNumber(flagArgs["-n"].front());
        if(commits.size() > n) commits.resize(n);
    }
    for(const Commit &c : commits) {
        std::cout << "commit " << c.hash() << std::endl;
        std::cout << "Date:    " << c.getTime() << std::endl;
        std::cout << "Message: " << c.getMsg() << std::endl;
        std::cout << std::endl;
    }
}
