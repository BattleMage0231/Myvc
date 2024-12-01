#include <iostream>
#include "init.h"

using namespace myvc::commands;

Init::Init(fs::path path, std::vector<std::string> rawArgs)
    : Command {std::move(path), std::move(rawArgs), false} {}

void Init::printHelpMessage() {
    std::cerr << "usage: myvc init" << std::endl;
}

void Init::createRules() {}

void Init::process() {
    if(fs::exists(path / ".myvc")) {
        throw command_error {"repository already exists"};
    } else {
        fs::create_directory(path / ".myvc");
        std::cout << "initialized repository at " << path << std::endl;
    }
}
