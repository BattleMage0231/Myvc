#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include "commit.h"

using namespace myvc::commands;

Commit::Commit(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Commit::printHelpMessage() {
    std::cerr << "usage: myvc commit [-m msg]" << std::endl;
}

void Commit::createRules() {
    Command::createRules();
    flagRules["-m"] = 1;
}

void Commit::process() {
    Index index = resolveIndex();
    if(index.getDiff().getChanges().empty()) {
        std::cout << "No changes to commit" << std::endl;
    } else {
        std::string msg;
        if(flagArgs.find("-m") != flagArgs.end()) {
            msg = flagArgs["-m"].front();
        } else {
            fs::remove(".myvc/COMMIT_EDITMSG");
            std::system("vim .myvc/COMMIT_EDITMSG");
            std::ifstream editedFile(".myvc/COMMIT_EDITMSG");
            std::getline(editedFile, msg);
        }
        time_t time = std::time(nullptr);
        auto head = store->getHead();
        std::set<Hash> parents;
        if(head) parents.insert(head.value().getCommit().getHash());
        myvc::Commit c { parents, index.getTree().getHash(), time, msg, store };
        c.store();
        if(head && head.value().isBranch()) {
            Branch b = head.value().getBranch().value();
            b.setCommit(c.getHash());
            b.store();
        } else if(head) {
            Head headVal = head.value();
            headVal.setState(c);
            headVal.store();
        } else {
            Branch main {"main", c.getHash(), store};
            main.store();
            Head newHead {main.getName(), store};
            newHead.store();
        }
        index.reset(index.getTree().getHash());
    }
}
