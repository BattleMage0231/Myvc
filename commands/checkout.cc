#include <algorithm>
#include <iostream>
#include <vector>
#include "checkout.h"

using namespace myvc;
using namespace myvc::commands;

Checkout::Checkout(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Checkout::printHelpMessage() {
    std::cerr << "usage: myvc checkout name" << std::endl;
}

void Checkout::process() {
    if(args.size() != 1) throw command_error {"illegal arguments"};
    ensureNoUncommitted();
    Index index = resolveIndex();
    Head head = resolveHead();
    Branch b = resolveBranch(args.at(0));
    index.reset((*b).getTree().hash());
    head.setState(b.getName());
    head.store();
    store->setWorkingTree((*head).getTree());
}
