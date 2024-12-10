#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include "cherrypick.h"
#include "merge.h"
#include "rebase.h"

using namespace myvc;
using namespace myvc::commands;

Cherrypick::Cherrypick(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Cherrypick::printHelpMessage() const {
    std::cerr << "usage: myvc cherry-pick commit" << std::endl;
}

void Cherrypick::process() {
    expectNumberOfArgs(1);
    expectCleanState();
    if(fs::exists(Rebase::rebaseInfoPath)) {
        throw command_error {"cannot cherry-pick while rebase is ongoing"};
    } else if(fs::exists(Merge::mergeInfoPath)) {
        throw command_error {"cannot cherry-pick while merge is ongoing"};
    }
    Commit c = resolveSymbol(args.at(0));
    if(c.getParents().size() > 1) {
        throw command_error {"cannot cherry-pick a merge commit"};
    }
    auto maybe = repo->cherrypick(c.hash());
    if(maybe) {
        std::vector<fs::path> conflicts = maybe.value();
        if(conflicts.empty()) {
            std::cout << "Cherry-pick completed successfully." << std::endl;
        } else {
            std::cout << "Conflicts detected, working directory modified." << std::endl;
            for(const auto &path : conflicts) {
                std::cout << "CONFLICT: " << path << std::endl;
            }
        }
    } else {
        std::cout << "Would have no effect." << std::endl;
    }
}
