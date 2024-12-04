#include <algorithm>
#include <iostream>
#include <vector>
#include "diff.h"
#include "../diff.h"
#include "../treediff.h"
#include "../tree.h"

using myvc::TreeDiff, myvc::Change, myvc::TreeChange, myvc::Hunk, myvc::Blob, myvc::Tree;
using namespace myvc::commands;

Diff::Diff(fs::path repoPath, std::vector<std::string> rawArgs)
    : Command {std::move(repoPath), std::move(rawArgs)} {}

void Diff::printHelpMessage() {
    std::cerr << "usage: myvc diff [--no-index] [--cached] commit1 [commit2]" << std::endl;
}

void Diff::createRules() {
    Command::createRules();
    flagRules["--no-index"] = 0;
    flagRules["--cached"] = 0;
}

void printHunk(const myvc::Hunk &hunk) {
    const std::vector<Change> &changes = hunk.getChanges();
    for(const auto &change : changes) {
        if(change.type == Change::Type::Add) {
            std::cout << '+';
        } else {
            std::cout << '-';
        }
        std::cout << change.content << std::endl;
    }
}

void printDiff(const myvc::Diff &diff) {
    const std::vector<std::string> &base = diff.getBase();
    const std::vector<Hunk> &hunks = diff.getHunks();
    if(hunks.empty()) {
        for(const auto &line : base) std::cout << line << std::endl;
        return;
    }
    printHunk(hunks.front());
    for(size_t i = 1; i < hunks.size(); ++i) {
        for(size_t j = hunks[i - 1].getEnd(); j < hunks[i].getIndex(); ++i) {
            std::cout << base[j] << std::endl;
        }
        printHunk(hunks[i]);
    }
}

void printTreeDiff(const TreeDiff &diff) {
    for(const auto &[path, change] : diff.getChanges()) {
        if(change.type == TreeChange::Type::Add) {
            std::cout << "added file " << path << std::endl;
            std::cout << "+++ theirs" << std::endl;
        } else if(change.type == TreeChange::Type::Delete) {
            std::cout << "deleted file ";
            std::cout << "--- ours" << std::endl;
        } else {
            std::cout << "modified file ";
            std::cout << "--- ours" << std::endl;
            std::cout << "+++ theirs" << std::endl;
        }
        printDiff(Blob::diff(change.oldBlob, change.newBlob));
    }
}

void Diff::process() {
    bool noIndex = flagArgs.find("--no-index") != flagArgs.end();
    bool cached = flagArgs.find("--cached") != flagArgs.end();
    if(noIndex && cached) {
        throw command_error {"can't pass both --no-index and --cached"};
    } else if(noIndex) {
        if(args.size() != 2) throw command_error {"diff requires two paths"};
        fs::path first = resolvePath(args.at(0)), second = resolvePath(args.at(1));
        ensureExists(first);
        ensureExists(second);
        if(!fs::is_regular_file(first) || !fs::is_regular_file(second)) {
            throw command_error {"diff requires two paths to files"};
        }
        std::cout << "--- " << first << std::endl;
        std::cout << "+++ " << second << std::endl;
        printDiff(Blob::diff(store->getBlobAt(first), store->getBlobAt(second)));
    } else if(cached) {
        Commit c = (args.size() == 1) ? resolveSymbol(args.at(0)) : *resolveHead();
        Tree commitTree = c.getTree(), indexTree = resolveIndex().getTree();
        printTreeDiff(Tree::diff(commitTree, indexTree));
    } else {
        if(args.size() == 2) {
            Commit a = resolveSymbol(args.at(0)), b = resolveSymbol(args.at(1));
            Tree at = a.getTree(), bt = b.getTree();
            printTreeDiff(Tree::diff(at, bt));
        } else if(args.size() == 1) {
            Commit a = resolveSymbol(args.at(0));
            Tree t = a.getTree(), workingTree = store->getWorkingTree();
            printTreeDiff(Tree::diff(t, workingTree));
        } else if(args.empty()) {
            Tree index = resolveIndex().getTree(), workingTree = store->getWorkingTree();
            printTreeDiff(Tree::diff(index, workingTree));
        } else {
            throw command_error {"invalid number of arguments"};
        }
    }
}
