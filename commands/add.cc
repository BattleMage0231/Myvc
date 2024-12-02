#include <algorithm>
#include <iostream>
#include <vector>
#include "add.h"

using namespace myvc::commands;

Add::Add(fs::path path, std::vector<std::string> rawArgs)
    : Command {std::move(path), std::move(rawArgs)} {}

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
        bool recursive = flagArgs.find("-r") != flagArgs.end();
        Tree workingTree = store->getWorkingTree();
        std::vector<fs::path> paths;
        for(const std::string &arg : args) {
            fs::path argPath = resolvePath(arg, false);
            fs::path rel = getRelative(argPath);
            if(!workingTree.getAtPath(rel) && !indexTree.getAtPath(rel)) {
                throw command_error {"path " + static_cast<std::string>(rel) + " did not match any files"};
            }
            paths.emplace_back(rel);
        }
        for(const fs::path &path : paths) {
            auto res1 = workingTree.getAtPath(path);
            auto res2 = indexTree.getAtPath(path);
            if(res2 && !res1) {
                // delete a file or directory from the index
                index.deleteEntry(path);
            } else if(res1) {
                if(std::holds_alternative<Tree>(res1.value())) {
                    // adding a directory to the index
                    Tree tree = std::get<Tree>(res1.value());
                    if(recursive) {
                        index.updateEntry(path, tree);
                    } else {
                        for(const auto &[k, node] : tree) {
                            if(node.isBlob()) {
                                index.updateEntry(path / k, std::get<Blob>(node.getData()));
                            }
                        }
                    }
                } else {
                    // adding a file to the index
                    index.updateEntry(path, std::get<Blob>(res1.value()));
                }
            }
            indexTree.reload();
        }
        index.store();
    }
}
