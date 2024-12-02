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

void Add::addDir(Index &index, const fs::path &base, const Tree &tree) {
    /*
    for(const auto &[k, node] : tree) {
        if(node.isBlob()) {
            Hash h = std::get<Blob>(node.getData()).getHash();
            index.addFile(base, h);
        }
    }
    */
}

void Add::addDirRecursive(Index &index, const fs::path &base, const Tree &tree) {
    /*
    for(const auto &[k, node] : tree) {
        if(node.isBlob()) {
            Hash h = std::get<Blob>(node.getData()).getHash();
            index.addFile(base, h);
        } else {
            addDirRecursive(index, base / k, std::get<Tree>(node.getData()));
        }
    }
    */
}

void Add::process() {
    /*
    if(args.size() == 0) {
        std::cout << "Nothing specified, nothing added." << std::endl;
    } else {
        std::vector<fs::path> paths;
        for(const std::string &arg : args) {
            fs::path argPath = resolvePath(arg);
            ensureIsFile(argPath);
            paths.emplace_back(getRelative(argPath));
        }
        Index index = resolveIndex();
        bool recursive = flagArgs.find("-r") != flagArgs.end();
        Tree workingTree = store->getWorkingTree();
        for(const fs::path &path : paths) {
            if(fs::is_directory(path)) {
                Tree t = std::get<Tree>(workingTree.getAtPath(path).value());
                if(recursive) addDirRecursive(index, path, t);
                else addDir(index, path, t);
            } else {
                Blob b = std::get<Blob>(workingTree.getAtPath(path).value());
                b.store();
                index.addFile(path, b.getHash());
            }
        }
        index.store();
    }
    */
}
