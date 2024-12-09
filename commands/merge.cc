#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include "merge.h"

using namespace myvc;
using namespace myvc::commands;

Merge::Merge(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Merge::printHelpMessage() const {
    std::cerr << "usage: myvc merge [--abort] [commit]" << std::endl;
}

void Merge::createRules() {
    Command::createRules();
    addFlagRule("--abort");
}

void Merge::process() {
    if(fs::exists(mergeInfoPath)) {
        if(hasFlag("--abort")) {
            expectNumberOfArgs(0);
            fs::remove(mergeInfoPath);
            std::cout << "Aborted." << std::endl;
            return;
        } else {
            throw command_error {"ongoing merge detected, use commit if you want to continue"};
        }
    } else if(hasFlag("--abort")) {
        throw command_error {"no ongoing merge to abort"};
    }
    expectNumberOfArgs(1);
    expectCleanState();
    Head &head = repo->getHead();
    Commit c = resolveSymbol(args.at(0));
    if(head.getCommit().hasParent(c)) {
        std::cout << "Already up to date." << std::endl;
    } else if(c.hasParent(head.getCommit())) {
        std::cout << "Fast forwarding..." << std::endl;
        repo->moveHeadSticky(c.hash());
        repo->setWorkingTree(c.getTree());
    } else {
        std::vector<fs::path> conflicts = repo->merge(c.hash());
        if(conflicts.empty()) {
            std::cout << "Merge conflict(s) detected." << std::endl << std::endl;
            for(const auto &path : conflicts) {
                std::cout << "CONFLICT: " << path << std::endl;
            }
            std::ofstream out {mergeInfoPath};
            out << c.hash();
        } else {
            std::cout << "Merging completed successfully." << std::endl;
        }
    }
}
