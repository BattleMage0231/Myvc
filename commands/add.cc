#include <algorithm>
#include <iostream>
#include <vector>
#include "add.h"

using namespace myvc::commands;

Add::Add(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Add::printHelpMessage() const {
    std::cerr << "usage: myvc add path1 ... pathn" << std::endl;
    std::cerr << "Adds the files or directories at the given paths to the index." << std::endl;
}

void Add::process() {
    if(args.size() == 0) {
        std::cout << "Nothing specified, nothing added." << std::endl;
    } else {
        Tree indexTree = repo->getIndex().getTree();
        Tree workingTree = repo->getWorkingTree();
        std::vector<fs::path> paths;
        for(const std::string &arg : args) {
            fs::path adjusted = resolvePath(arg);
            bool inIndex = indexTree.getAtPath(adjusted).has_value();
            bool inWorkingTree = workingTree.getAtPath(adjusted).has_value();
            if(adjusted != "." && !inIndex && !inWorkingTree) {
                throw command_error {"path " + static_cast<std::string>(adjusted) + " did not match any files"};
            }
            paths.emplace_back(std::move(adjusted));
        }
        repo->addToIndex(paths);
    }
}
