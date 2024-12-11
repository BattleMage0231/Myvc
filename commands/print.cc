#include <algorithm>
#include <iostream>
#include <vector>
#include <sstream>
#include "print.h"
#include "../tree.h"

using namespace myvc;
using namespace myvc::commands;

Print::Print(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Print::printHelpMessage() const {
    std::cerr << "usage: myvc print type [id]" << std::endl;
    std::cerr << "Displays an entity of type head, index, commit, blob, tree, wt, branch." << std::endl;
}

static std::string pad(size_t n) {
    std::stringstream ss;
    for(size_t i = 0; i < n; ++i) ss << ' ';
    return ss.str();
}

void printTree(const Tree &t, size_t depth = 1) {
    for(const auto &[k, v] : t) {
        auto data = v.getData();
        if(std::holds_alternative<Blob>(data)) {
            std::cout << pad(depth) << "Blob with name " << k << " at " << std::get<Blob>(data).hash() << std::endl;
        } else {
            Tree t = std::get<Tree>(data);
            std::cout << pad(depth) << "Tree with name " << k << " at " << t.hash() << std::endl;
            printTree(t, depth + 1);
        }
    }
}

void Print::process() {
    if(args.size() == 0) return;
    std::string thing = args.at(0);
    if(thing == "head") {
        expectNumberOfArgs(1);
        Head &head = repo->getHead();
        if(head.hasState()) {
            auto val = head.get();
            if(std::holds_alternative<std::reference_wrapper<Branch>>(val)) {
                std::cout << "On branch " << std::get<std::reference_wrapper<Branch>>(val).get().getName() << std::endl;
            } else {
                std::cout << "Detached at " << std::get<Commit>(val).hash() << std::endl;
            }
        } else {
            std::cout << "No head" << std::endl;
        }
    } else if(thing == "index" || thing == "wt" || thing == "tree") {
        Tree t;
        if(thing == "index") {
            expectNumberOfArgs(1);
            t = repo->getIndex().getTree();
        } else if(thing == "wt") {
            expectNumberOfArgs(1);
            t = repo->getWorkingTree();
        } else {
            expectNumberOfArgs(2);
            t = repo->getTree(resolveHash(args.at(1))).value();
        }
        std::cout << "Tree at " << t.hash() << std::endl;
        printTree(t);
    } else if(thing == "commit") {
        expectNumberOfArgs(2);
        Commit c = resolveSymbol(args.at(1));
        std::cout << "Commit at " << c.hash() << std::endl;
        std::cout << "Parents:" << std::endl;
        for(const auto &h : c.getParentHashes()) {
            std::cout << h << std::endl;
        }
        std::cout << "Tree: " << c.getTree().hash() << std::endl;
        std::cout << "Time: " << c.getTime() << std::endl;
        std::cout << "Msg: \"" << c.getMsg() << "\"" << std::endl;
    } else if(thing == "blob") {
        expectNumberOfArgs(2);
        Blob b = repo->getBlob(resolveHash(args.at(1))).value();
        std::cout << "Blob at " << b.hash() << std::endl;
        for(char c : b.getData()) std::cout << c;
        std::cout << std::endl;
    } else if(thing == "branch") {
        expectNumberOfArgs(2);
        std::string name = args.at(1);
        Branch b = resolveBranch(name);
        std::cout << "Branch at " << b.getName() << std::endl;
        std::cout << "Points to " << b.getCommit().hash() << std::endl;
    } else {
        throw command_error {"no rule to print " + thing};
    }
}
