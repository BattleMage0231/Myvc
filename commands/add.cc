#include <algorithm>
#include <iostream>
#include <vector>
#include "add.h"

using namespace myvc::commands;

Add::Add(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Add::printHelpMessage() {
    std::cerr << "usage: myvc add [-r] path1 ... pathn" << std::endl;
}

void Add::createRules() {
    Command::createRules();
    flagRules["-r"] = 0;
}

void Add::process() {
    if(args.size() == 0) {
        std::cout << "Nothing specified, nothing added." << std::endl;
    } else {
        Index index = resolveIndex();
        Tree indexTree = index.getTree();
        std::vector<fs::path> paths;
        for(const std::string &arg : args) {
            fs::path rel = getRelative(resolvePath(arg));
            if(!fs::exists(rel) && !indexTree.getAtPath(rel)) {
                throw command_error {"path " + static_cast<std::string>(rel) + " did not match any files"};
            }
            paths.emplace_back(rel);
        }
        for(const fs::path &path : paths) {
            auto indexNode = indexTree.getAtPath(path);
            if(indexNode && !fs::exists(path)) {
                // delete file or directory
                index.deleteEntry(path);
            } else if(fs::exists(path)) {
                if(fs::is_directory(path)) {
                    Tree tree = store->getTreeAt(path);
                    if(tree.getNodes().empty()) {
                        index.deleteEntry(path);
                    } else {
                        index.updateEntry(path, tree);
                    }
                } else {
                    // add file
                    index.updateEntry(path, store->getBlobAt(path));
                }
            }
            indexTree.reload();
        }
    }
}
