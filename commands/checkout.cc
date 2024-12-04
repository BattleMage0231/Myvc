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
    Index index = resolveIndex();
    Head head = resolveHead();
    Branch b = resolveBranch(args.at(0));
    TreeDiff diff1 = Tree::diff(store->getWorkingTree(), index.getTree()), diff2 = Tree::diff(index.getTree(), (*head).getTree());
    if(!diff1.getChanges().empty() || !diff2.getChanges().empty()) {
        throw command_error {"branch cannot be checked out with uncommitted changes"};
    }
    index.reset((*b).getTree());
    head.setState(b.getName());
    head.store();
    store->setWorkingTree((*head).getTree());
}
