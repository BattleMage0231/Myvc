#include <algorithm>
#include <iostream>
#include <vector>
#include "branch.h"

using namespace myvc::commands;

Branch::Branch(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Branch::printHelpMessage() {
    std::cerr << "usage: myvc branch [-d] [name]" << std::endl;
}

void Branch::createRules() {
    Command::createRules();
    flagRules["-d"] = 0;
}

void Branch::process() {
    if(flagArgs.find("-d") != flagArgs.end()) {
        if(args.size() != 1) throw command_error {"needs branch name to delete"};
        store->deleteBranch(args.at(0));
    } else if(args.empty()) {
        // list branches
        auto head = store->getHead();
        std::vector<myvc::Branch> branches = store->getAllBranches();
        for(const auto &branch : branches) {
            if(head && head.value().getBranch() && head.value().getBranch().value().getName() == branch.getName()) {
                std::cout << "*";
            } else {
                std::cout << " ";
            }
            std::cout << " " << branch.getName() << std::endl;
        }
    } else if(args.size() == 1) {
        std::string name = args.at(0);
        if(store->getBranch(name)) {
            throw command_error {"branch " + name + " already exists"};
        }
        myvc::Branch b {name, (*resolveHead()).hash(), store};
        store->updateBranch(b);
    } else {
        throw command_error {"too many arguments"};
    }
}
