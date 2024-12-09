#include <algorithm>
#include <iostream>
#include <vector>
#include "reset.h"
#include "../hash.h"

using namespace myvc::commands;

Reset::Reset(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Reset::printHelpMessage() const {
    std::cerr << "usage: myvc reset [--soft] [--mixed] [--hard] commit" << std::endl;
}

void Reset::createRules() {
    Command::createRules();
    addFlagRule("--soft");
    addFlagRule("--mixed");
    addFlagRule("--hard");
}

void Reset::process() {
    expectNumberOfArgs(1);
    bool mixed = hasFlag("--mixed");
    bool hard = hasFlag("--hard");
    // move head
    repo->moveHeadSticky(resolveSymbol(args.at(0)));
    // reset index
    Head &head = repo->getHead();
    if(mixed || hard) {
        repo->getIndex().setTree(head.getCommit().getTree().hash());
    }
    // reset working directory
    if(hard) {
        repo->setWorkingTree(head.getCommit().getTree());
    }
}
