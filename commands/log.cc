#include <algorithm>
#include <iostream>
#include <vector>
#include <chrono>
#include <format>
#include "log.h"

using namespace myvc::commands;

Log::Log(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Log::printHelpMessage() const {
    std::cerr << "usage: myvc log [-n NUM] [commit]" << std::endl;
}

void Log::createRules() {
    Command::createRules();
    addFlagRule("-n", 1);
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
    std::vector<Commit> commits = Commit::getAllReachable(c);
    std::sort(commits.begin(), commits.end(), [](const Commit &a, const Commit &b) {
        return a.getTime() > b.getTime();
    });
    if(hasFlag("-n")) {
        size_t n = resolveNumber(getFlagArgs("-n").at(0));
        if(commits.size() > n) commits.resize(n);
    }
    for(const Commit &c : commits) {
        std::cout << "commit " << c.hash() << std::endl;
        auto timePoint = std::chrono::system_clock::from_time_t(c.getTime());
        auto time = std::chrono::current_zone()->to_local(timePoint);
        std::cout << "Date:    " << std::format("{:%a %b %d %H:%M:%S %Y}", time) << std::endl;
        std::cout << "Message: " << c.getMsg() << std::endl;
        std::cout << std::endl;
    }
}
