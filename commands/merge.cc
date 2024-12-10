#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include "merge.h"
#include "rebase.h"
#include "../fileops.h"

using namespace myvc;
using namespace myvc::commands;

Merge::Merge(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Merge::printHelpMessage() const {
    std::cerr << "usage: myvc merge [--abort] [commit]" << std::endl;
    std::cerr << "Merges a commit with HEAD, creating a new merge commit." << std::endl;
    std::cerr << "merge commit creates a new merge commit from HEAD and the given commit." << std::endl;
    std::cerr << "merge --abort stops the current merge." << std::endl;
}

void Merge::createRules() {
    Command::createRules();
    addFlagRule("--abort");
}

void Merge::process() {
    if(fs::exists(mergeInfoPath)) {
        if(hasFlag("--abort")) {
            expectNumberOfArgs(0);
            fileops::remove(mergeInfoPath);
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
    if(fs::exists(Rebase::rebaseInfoPath)) {
        throw command_error {"cannot merge while rebase is ongoing"};
    }
    Head &head = repo->getHead();
    Commit c = resolveSymbol(args.at(0));
    if(head.getCommit().hasParent(c)) {
        std::cout << "Already up to date." << std::endl;
    } else if(c.hasParent(head.getCommit())) {
        std::cout << "Fast forwarding..." << std::endl;
        repo->moveHeadSticky(c.hash());
        repo->setWorkingTree(c.getTree());
    } else {
        auto maybe = repo->cherrypick(c.hash());
        if(maybe) {
            std::vector<fs::path> conflicts = maybe.value();
            if(conflicts.empty()) {
                std::cout << "Merge completed successfully." << std::endl;
            } else {
                std::cout << "Merge conflicts detected, working directory modified." << std::endl;
                for(const auto &path : conflicts) {
                    std::cout << "CONFLICT: " << path << std::endl;
                }
            }
        } else {
            std::cout << "Would have no effect." << std::endl;
        }
    }
}
