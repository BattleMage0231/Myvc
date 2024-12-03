#include <iostream>
#include "init.h"

using namespace myvc::commands;

Init::Init(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs), false} {}

void Init::printHelpMessage() {
    std::cerr << "usage: myvc init" << std::endl;
}

void Init::process() {
    if(RepositoryStore::createAt(repoPath)) {
        std::cout << "initialized repository at " << repoPath << std::endl;
    } else {
        throw command_error {"repository already exists"};
    }
}
