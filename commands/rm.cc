#include <algorithm>
#include <iostream>
#include <vector>
#include <variant>
#include "rm.h"

using namespace myvc::commands;

Rm::Rm(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Rm::printHelpMessage() {
    std::cerr << "usage: myvc rm [-r] path1 ... pathn" << std::endl;
}

void Rm::createRules() {
    Command::createRules();
    flagRules["-r"] = 0;
    flagRules["--cached"] = 0;
}

void Rm::process() {
    if(args.size() == 0) {
        std::cout << "Nothing specified, nothing rmed." << std::endl;
    } else {
        Index index = resolveIndex();
        Tree indexTree = index.getTree();
        bool recursive = flagArgs.find("-r") != flagArgs.end();
        bool cached = flagArgs.find("--cached") != flagArgs.end();
        std::vector<fs::path> paths;
        for(const std::string &arg : args) {
            fs::path rel = getRelative(resolvePath(arg));
            auto entity = indexTree.getAtPath(rel);
            if(!entity) {
                throw command_error {"path " + static_cast<std::string>(rel) + " did not match any files in the index"};
            } else if(std::holds_alternative<Tree>(entity.value()) && !recursive) {
                throw command_error {"need to pass -r to rm the directory " + static_cast<std::string>(rel)};
            }
            paths.emplace_back(rel);
        }
        for(const fs::path &path : paths) {
            index.deleteEntry(path);
            if(!cached) fs::remove_all(path);
        }
    }
}
