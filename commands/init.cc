#include <iostream>
#include "init.h"

using namespace myvc::commands;

Init::Init(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs), false} {}

void Init::printHelpMessage() const {
    std::cerr << "usage: myvc init" << std::endl;
    std::cerr << "Initializes a new repository." << std::endl;
}

void Init::process() {
    expectNumberOfArgs(0);
    if(RepositoryStore::createAt(repoPath)) {
        std::cout << "initialized repository at " << repoPath << std::endl;
    } else {
        throw command_error {"repository already exists"};
    }
}
