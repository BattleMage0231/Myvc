#include <algorithm>
#include <iostream>
#include <vector>
#include <variant>
#include "rm.h"

using namespace myvc::commands;

Rm::Rm(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Rm::printHelpMessage() const {
    std::cerr << "usage: myvc rm [-r] [--cached] path1 ... pathn" << std::endl;
    std::cerr << "Removes files from the index and deletes them in the working directory." << std::endl;
    std::cerr << "rm path1 ... pathn unstages and deletes the given files." << std::endl;
    std::cerr << "rm -r path1 ... pathn unstages and deletes the given files and directories." << std::endl;
    std::cerr << "rm --cached path1 ... pathn unstages the given files without deleting them." << std::endl;
}

void Rm::createRules() {
    Command::createRules();
    addFlagRule("-r");
    addFlagRule("--cached");
}

void Rm::process() {
    if(args.size() == 0) {
        std::cout << "Nothing specified, nothing rmed." << std::endl;
    } else {
        Tree indexTree = repo->getIndex().getTree();
        Tree workingTree = repo->getWorkingTree();
        bool recursive = hasFlag("-r"), cached = hasFlag("--cached");
        std::vector<fs::path> paths;
        for(const std::string &arg : args) {
            fs::path adjusted = resolvePath(arg);
            auto entity = indexTree.getAtPath(adjusted);
            if(!entity) {
                throw command_error {"path " + static_cast<std::string>(adjusted) + " did not match any files in the index"};
            } else if(std::holds_alternative<Tree>(entity.value()) && !recursive) {
                throw command_error {"need to pass -r to remove the directory " + static_cast<std::string>(adjusted)};
            }
            paths.emplace_back(std::move(adjusted));
        }
        repo->removeFromIndex(paths, cached);
    }
}
