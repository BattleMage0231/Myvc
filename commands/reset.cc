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
    std::cerr << "Resets the state of the repository to a given commit." << std::endl;
    std::cerr << "reset commit or reset --soft commit moves HEAD to the given commit." << std::endl;
    std::cerr << "reset --mixed commit moves HEAD and resets the index." << std::endl;
    std::cerr << "reset --hard commit moves HEAD, resets the index, and resets the working directory." << std::endl;
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
    Head &head = repo->getHead();
    head.setCommit(resolveSymbol(args.at(0)));
    // reset index
    if(mixed || hard) {
        repo->getIndex().setTree(head.getCommit().getTree().hash());
    }
    // reset working directory
    if(hard) {
        repo->setWorkingTree(head.getCommit().getTree());
    }
}
