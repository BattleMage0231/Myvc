#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include "rebase.h"
#include "../errors.h"

using namespace myvc;
using namespace myvc::commands;

Rebase::Rebase(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Rebase::printHelpMessage() {
    std::cerr << "usage: myvc rebase [--continue] [--abort] [commit]" << std::endl;
}

void Rebase::createRules() {
    Command::createRules();
    flagRules["--continue"] = 0;
    flagRules["--abort"] = 0;
}

bool Rebase::rebase_from(const Commit &start, const Commit &cur) {
    if(start == cur) return true;
    if(cur.getParents().size() > 1) throw not_implemented {};
    Commit parent = cur.getParents().at(0);
    if(!rebase_from(start, parent)) return false;
    // basically cherry pick
    throw not_implemented {};
}

void Rebase::process() {
    if(flagArgs.find("--abort") != flagArgs.end()) {
        fs::remove(".myvc/REBASE_INFO");
        std::cout << "Aborted." << std::endl;
        return;
    }
    if(flagArgs.find("--continue") != flagArgs.end()) {
        if(!fs::exists(".myvc/REBASE_INFO")) {
            throw command_error {"nothing to continue"};
        }
        Hash start, cur;
        std::ifstream in {".myvc/REBASE_INFO", std::ios::binary};
        start.read(in);
        cur.read(in);
        rebase_from(store->getCommit(start).value(), store->getCommit(cur).value());
        return;
    }
    if(args.size() != 1) throw command_error {"invalid number of arguments"};
    ensureNoUncommitted();
    Head head = resolveHead();
    Commit c = resolveSymbol(args.at(0));
    auto lca = Commit::getLCA(*head, c);
    if(!lca) throw command_error {"no lca exists between the two commits"};
    Commit lcaCommit = lca.value();
    rebase_from(lcaCommit, c);
}
