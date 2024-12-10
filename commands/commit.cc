#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include "commit.h"
#include "../serialize.h"
#include "../store.h"
#include "merge.h"

using namespace myvc::commands;

Commit::Commit(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Commit::printHelpMessage() const {
    std::cerr << "usage: myvc commit [-m msg]" << std::endl;
}

void Commit::createRules() {
    Command::createRules();
    addFlagRule("-m", 1);
}

void Commit::process() {
    expectNumberOfArgs(0);
    Index &index = repo->getIndex();
    Head &head = repo->getHead();
    Tree tip = head.hasState() ? head.getCommit().getTree() : Tree {};
    if(index.getTree() == tip) {
        std::cout << "No changes to commit" << std::endl;
    } else {
        std::string msg;
        if(hasFlag("-m")) {
            msg = getFlagArgs("-m").at(0);
        } else {
            fs::remove(editMessagePath);
            std::system((editorName + " " + editMessagePath).c_str());
            std::ifstream editedFile(editMessagePath);
            std::getline(editedFile, msg);
        }
        std::set<Hash> otherParents;
        if(fs::exists(Merge::mergeInfoPath)) {
            std::ifstream in {Merge::mergeInfoPath};
            std::string s; in >> s;
            otherParents.insert(Hash {s});
        }
        repo->commitIndex(std::move(msg), std::move(otherParents));
        fs::remove(Merge::mergeInfoPath);
    }
}
