#include <algorithm>
#include <iostream>
#include <vector>
#include "branch.h"

using namespace myvc::commands;

Branch::Branch(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Branch::printHelpMessage() const {
    std::cerr << "usage: myvc branch [-d] [name]" << std::endl;
}

void Branch::createRules() {
    Command::createRules();
    addFlagRule("-d");
}

void Branch::process() {
    if(!hasFlag("-d")) {
        expectNumberOfArgs(1);
        Head &h = repo->getHead();
        if(h.hasState() && std::holds_alternative<std::reference_wrapper<myvc::Branch>>(h.get())) {
            myvc::Branch &b = std::get<std::reference_wrapper<myvc::Branch>>(h.get());
            if(b.getName() == args.at(0)) {
                h.setCommit(b.getCommit());
            }
        }
        repo->deleteBranch(args.at(0));
    } else if(args.empty()) {
        // list branches
        Head &h = repo->getHead();
        std::optional<std::string> headBranch;
        if(h.hasState()) {
            auto data = h.get();
            if(std::holds_alternative<std::reference_wrapper<myvc::Branch>>(data)) {
                headBranch = std::get<std::reference_wrapper<myvc::Branch>>(data).get().getName();
            }
        }
        std::vector<std::reference_wrapper<myvc::Branch>> branches = repo->getAllBranches();
        for(const auto &branch : branches) {
            if(headBranch && headBranch.value() == branch.get().getName()) {
                std::cout << "*";
            } else {
                std::cout << " ";
            }
            std::cout << " " << branch.get().getName() << std::endl;
        }
    } else if(args.size() == 1) {
        std::string name = args.at(0);
        if(repo->getBranch(name)) {
            throw command_error {"branch " + name + " already exists"};
        }
        if(!repo->getHead().hasState()) {
            throw command_error {"nonexistent HEAD"};
        }
        repo->createBranch(name, repo->getHead().getCommit());
    } else {
        throw command_error {"too many arguments"};
    }
}
