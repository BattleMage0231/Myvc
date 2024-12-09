#include <algorithm>
#include <iostream>
#include <vector>
#include "checkout.h"

using namespace myvc;
using namespace myvc::commands;

Checkout::Checkout(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Checkout::printHelpMessage() const {
    std::cerr << "usage: myvc checkout name" << std::endl;
}

void Checkout::process() {
    expectNumberOfArgs(1);
    expectCleanState();
    std::string branch = args.at(0);
    Head &head = repo->getHead();
    resolveBranch(branch);
    if(head.hasState() && std::holds_alternative<std::reference_wrapper<Branch>>(head.get())) {
        std::string c = std::get<std::reference_wrapper<Branch>>(head.get()).get().getName();
        if(branch == c) {
            std::cout << "Already on " << branch << std::endl;
            return;
        }
    }
    repo->checkout(branch);
}
