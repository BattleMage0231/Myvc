#include <algorithm>
#include <iostream>
#include <vector>
#include "reset.h"
#include "../hash.h"

using namespace myvc::commands;

Reset::Reset(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Reset::printHelpMessage() {
    std::cerr << "usage: myvc reset [--soft] [--mixed] [--hard] commit" << std::endl;
}

void Reset::createRules() {
    Command::createRules();
    flagRules["--soft"] = 0;
    flagRules["--mixed"] = 0;
    flagRules["--hard"] = 0;
}

void Reset::process() {
    bool mixed = flagArgs.find("--mixed") != flagArgs.end();
    bool hard = flagArgs.find("--hard") != flagArgs.end();
    if(args.size() != 1) throw command_error {"incorrect number of arguments"};
    // move head
    Head head = resolveHead();
    if(store->getBranch(args.at(0))) {
        head.setState(args.at(0));
    } else {
        head.setState(resolveSymbol(args.at(0)).getHash());
    }
    head.store();
    // reset index
    Index index = resolveIndex();
    if(mixed || hard) {
        index.reset((*head).getHash());
    } else {
        index.updateBase((*head).getHash());
    }
    // reset working directory
    if(hard) {
        store->setWorkingTree((*head).getTree());
    }
}
