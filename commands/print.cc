#include <algorithm>
#include <iostream>
#include <vector>
#include "print.h"
#include "../tree.h"

using namespace myvc;
using namespace myvc::commands;

Print::Print(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Print::printHelpMessage() {
    std::cerr << "usage: myvc print type [hash]" << std::endl;
}

std::string pad(size_t n) {
    std::string s;
    for(size_t i = 0; i < n; ++i) s += ' ';
    return s;
}

void printTree(const Tree &t, size_t depth = 1) {
    for(const auto &[k, v] : t) {
        if(v.isBlob()) {
            std::cout << pad(depth) << "Blob with name " << k << " at " << std::get<Blob>(v.getData()).hash() << std::endl;
        } else {
            Tree t = std::get<Tree>(v.getData());
            std::cout << pad(depth) << "Tree with name " << k << " at " << t.hash() << std::endl;
            printTree(t, depth + 1);
        }
    }
}

void Print::process() {
    std::string thing = args.at(0);
    if(thing == "index") {
        auto index = store->getIndex();
        if(index) {
            Tree base = index.value().getBase(), tree = index.value().getTree();
            std::cout << "Index's tree at " << tree.hash() << std::endl;
            std::cout << "Index's base tree at " << base.hash() << std::endl;
        } else {
            std::cout << "No index" << std::endl;
        }
    } else if(thing == "head") {
        auto head = store->getHead();
        if(head) {
            Head h = head.value();
            if(h.isBranch()) {
                std::cout << "On branch " << h.getBranch().value().getName() << std::endl;
            } else {
                std::cout << "Detached at " << (*h).hash() << std::endl;
            }
        } else {
            std::cout << "No head" << std::endl;
        }
    } else if(thing == "wt" || thing == "tree") {
        Tree t;
        if(thing == "wt") {
            t = store->getWorkingTree();
        } else {
            std::string partial = args.at(1);
            auto full = store->resolvePartialObjectHash(partial);
            if(!full) {
                std::cout << "Hash does not exist or is not unique" << std::endl;
                return;
            }
            t = store->getTree(*full).value();
        }
        std::cout << "Tree at " << t.hash() << std::endl;
        printTree(t);
    } else if(thing == "commit") {
        std::string partial = args.at(1);
        auto full = store->resolvePartialObjectHash(partial);
        if(!full) {
            std::cout << "Hash does not exist or is not unique" << std::endl;
            return;
        }
        Commit c = store->getCommit(*full).value();
        std::cout << "Commit at " << c.hash() << std::endl;
        std::cout << "Parents:" << std::endl;
        for(const auto &h : c.getParentHashes()) {
            std::cout << h << std::endl;
        }
        std::cout << "Tree: " << c.getTree().hash() << std::endl;
        std::cout << "Time: " << c.getTime() << std::endl;
        std::cout << "Msg: \"" << c.getMsg() << "\"" << std::endl;
    } else if(thing == "blob") {
        std::string partial = args.at(1);
        auto full = store->resolvePartialObjectHash(partial);
        if(!full) {
            std::cout << "Hash does not exist or is not unique" << std::endl;
            return;
        }
        Blob b = store->getBlob(*full).value();
        std::cout << "Blob at " << b.hash() << std::endl;
        for(char c : b.getData()) std::cout << c;
        std::cout << std::endl;
    } else if(thing == "branch") {
        std::string name = args.at(1);
        Branch b = store->getBranch(name).value();
        std::cout << "Branch at " << b.getName() << std::endl;
        std::cout << "Points to " << b.getCommit().hash() << std::endl;
    }
}
