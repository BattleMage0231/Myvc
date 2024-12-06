#include <algorithm>
#include <iostream>
#include <vector>
#include "status.h"

using namespace myvc::commands;

Status::Status(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Status::printHelpMessage() {
    std::cerr << "usage: myvc status" << std::endl;
}

void Status::process() {
    // display head
    auto head = store->getHead();
    if(head) {
        Head headVal = head.value();
        if(headVal.isBranch()) {
            std::cout << "On branch " << headVal.getBranch().value().getName() << std::endl;
        } else {
            std::cout << "HEAD detached at " << (*headVal).hash() << std::endl;
        }
    } else {
        std::cout << "No commits yet" << std::endl;
    }
    std::cout << std::endl;
    // display index files
    Index index = resolveIndex();
    auto changes = index.getDiff().getChanges();
    if(changes.empty()) {
        std::cout << "Nothing to be committed" << std::endl;
    } else {
        std::cout << "Changes to be committed:" << std::endl;
        for(const auto &[path, change] : changes) {
            std::cout << "    ";
            if(change.type == TreeChange::Type::Add) {
                std::cout << "new file: ";
            } else if(change.type == TreeChange::Type::Delete) {
                std::cout << "deleted file: ";
            } else {
                std::cout << "modified file: ";
            }
            std::cout << path << std::endl;
        }
    }
    std::cout << std::endl;
    // display unstaged changes
    Tree indexTree = index.getTree();
    Tree workingTree = store->getWorkingTree();
    TreeDiff diff {indexTree.getAllFiles(), workingTree.getAllFiles()};
    auto wtChanges = diff.getChanges();
    if(wtChanges.empty()) {
        std::cout << "No untracked files" << std::endl;
    } else {
        std::cout << "Untracked files:" << std::endl;
        for(const auto &[path, change] : wtChanges) {
            std::cout << "    " << path << std::endl;
        }
    }
}
