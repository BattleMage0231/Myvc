#include <algorithm>
#include <iostream>
#include <vector>
#include "diff.h"
#include "../diff.h"
#include "../treediff.h"

using myvc::TreeDiff, myvc::Change, myvc::TreeChange, myvc::Hunk, myvc::Blob, myvc::Tree;
using namespace myvc::commands;

Diff::Diff(fs::path basePath, std::vector<std::string> rawArgs)
    : Command {std::move(basePath), std::move(rawArgs)} {}

void Diff::printHelpMessage() const {
    std::cerr << "usage: myvc diff [--no-index] [--cached] commit1 [commit2]" << std::endl;
}

void Diff::createRules() {
    Command::createRules();
    addFlagRule("--no-index");
    addFlagRule("--cached");
}

static void printHunk(const myvc::Hunk &hunk) {
    for(const auto &change : hunk.changes) {
        if(change.type == Change::Type::Add) {
            std::cout << '+';
        } else {
            std::cout << '-';
        }
        std::cout << change.content << std::endl;
    }
}

static void printDiff(const myvc::Diff &diff) {
    const std::vector<std::string> &base = diff.getBase();
    const std::vector<Hunk> &hunks = diff.getHunks();
    if(hunks.empty()) {
        for(const auto &line : base) std::cout << line << std::endl;
        return;
    }
    printHunk(hunks.front());
    for(size_t i = 1; i < hunks.size(); ++i) {
        for(size_t j = hunks[i - 1].end; j < hunks[i].index; ++i) {
            std::cout << base[j] << std::endl;
        }
        printHunk(hunks[i]);
    }
}

static void printTreeDiff(const TreeDiff &diff) {
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
    bool noIndex = hasFlag("--no-index"), cached = hasFlag("--cached");
    if(noIndex && cached) {
        throw command_error {"can't pass both --no-index and --cached"};
    } else if(noIndex) {
        expectNumberOfArgs(2);
        fs::path first = resolvePath(args.at(0)), second = resolvePath(args.at(1));
        expectIsFile(first);
        expectIsFile(second);
        std::cout << "--- " << first << std::endl;
        std::cout << "+++ " << second << std::endl;
        printDiff(Blob::diff(repo->getBlobAt(first).value(), repo->getBlobAt(second).value()));
    } else if(cached) {
        Commit c;
        if(args.empty()) {
            c = repo->getHead().getCommit();
        } else {
            expectNumberOfArgs(1);
            c = resolveSymbol(args.at(0));
        }
        printTreeDiff(Tree::diff(c.getTree(), repo->getIndex().getTree()));
    } else {
        if(args.empty()) {
            printTreeDiff(Tree::diff(repo->getIndex().getTree(), repo->getWorkingTree()));
        } else if(args.size() == 1) {
            Commit a = resolveSymbol(args.at(0));
            printTreeDiff(Tree::diff(a.getTree(), repo->getWorkingTree()));
        } else {
            expectNumberOfArgs(2);
            Commit a = resolveSymbol(args.at(0)), b = resolveSymbol(args.at(1));
            printTreeDiff(Tree::diff(a.getTree(), b.getTree()));
        }
    }
}
