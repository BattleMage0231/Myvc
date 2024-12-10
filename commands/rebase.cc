#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include "rebase.h"
#include "merge.h"

using namespace myvc;
using namespace myvc::commands;

Rebase::Rebase(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Rebase::printHelpMessage() const {
    std::cerr << "usage: myvc rebase [--continue] [--abort] [commit]" << std::endl;
    std::cerr << "Runs cherry-pick for every commit starting from the base of HEAD and the given commit." << std::endl;
    std::cerr << "rebase commit rebases starting from a given commit and HEAD." << std::endl;
    std::cerr << "rebase --continue continues the ongoing rebase." << std::endl;
    std::cerr << "rebase --abort stops the ongoing rebase." << std::endl;
}

void Rebase::createRules() {
    Command::createRules();
    addFlagRule("--continue");
    addFlagRule("--abort");
}

void Rebase::process() {
    std::vector<Hash> chain;
    if(fs::exists(rebaseInfoPath)) {
        if(hasFlag("--abort")) {
            expectNumberOfArgs(0);
            fs::remove(rebaseInfoPath);
            std::cout << "Aborted." << std::endl;
            return;
        } else if(hasFlag("--continue")) {
            expectNumberOfArgs(0);
            std::ifstream in {rebaseInfoPath};
            size_t n; in >> n;
            for(size_t i = 0; i < n; ++i) {
                std::string s; in >> s;
                chain.emplace_back(Hash {s});
            }
        } else {
            throw command_error {"ongoing rebase detected, use --continue if you want to continue"};
        }
    } else if(hasFlag("--abort") || hasFlag("--continue")) {
        throw command_error {"no ongoing rebase"};
    } else {
        if(fs::exists(Merge::mergeInfoPath)) {
            throw command_error {"cannot rebase while merge is ongoing"};
        }
        expectNumberOfArgs(1);
        expectCleanState();
        Commit c = resolveSymbol(args.at(0));
        Commit base = Commit::getLCA(c, repo->getHead().getCommit());
        for(const Commit &c : c.getParentChain(base)) {
            chain.emplace_back(c.hash());
        }
        chain.erase(chain.begin());
    }
    for(size_t i = 0; i < chain.size(); ++i) {
        std::vector<fs::path> res = repo->cherrypick(chain.at(i)).value();
        if(!res.empty()) {
            std::cout << "Rebase conflicts detected, working directory modified." << std::endl;
            for(const auto &path : res) {
                std::cout << "CONFLICT: " << path << std::endl;
            }
            std::ofstream out {rebaseInfoPath};
            out << (chain.size() - i - 1) << std::endl;
            for(size_t j = i + 1; j < chain.size(); ++j) {
                out << chain.at(j) << std::endl;
            }
            return;
        }
    }
    std::cout << "Rebase completed successfully" << std::endl;
}
