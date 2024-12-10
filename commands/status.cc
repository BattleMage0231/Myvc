#include <algorithm>
#include <iostream>
#include <vector>
#include "status.h"

using namespace myvc::commands;

Status::Status(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Status::printHelpMessage() const {
    std::cerr << "usage: myvc status" << std::endl;
    std::cerr << "Displays the current status of HEAD, the index, and the working directory." << std::endl;
}

void Status::process() {
    expectNumberOfArgs(0);
    // display head
    Head &h = repo->getHead();
    if(h.hasState()) {
        auto state = h.get();
        if(std::holds_alternative<std::reference_wrapper<Branch>>(state)) {
            std::cout << "On branch " << std::get<std::reference_wrapper<Branch>>(state).get().getName() << std::endl;
        } else if(std::holds_alternative<Commit>(state)) {
            std::cout << "HEAD detached at " << std::get<Commit>(state).hash() << std::endl;
        }
    } else {
        std::cout << "No commits yet" << std::endl;
    }
    std::cout << std::endl;
    // display index files
    Index &index = repo->getIndex();
    Tree tip = h.hasState() ? h.getCommit().getTree() : Tree {};
    TreeDiff changes = Tree::diff(tip, index.getTree());
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
    Tree workingTree = repo->getWorkingTree();
    TreeDiff wtChanges = Tree::diff(index.getTree(), workingTree);
    if(wtChanges.empty()) {
        std::cout << "No untracked files" << std::endl;
    } else {
        std::cout << "Untracked files:" << std::endl;
        for(const auto &[path, change] : wtChanges) {
            std::cout << "    " << path << std::endl;
        }
    }
}
