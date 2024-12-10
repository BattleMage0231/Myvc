#include <filesystem>
#include <memory>
#include <iostream>
#include <stdexcept>

#include "commands/command.h"
#include "commands/init.h"
#include "commands/log.h"
#include "commands/add.h"
#include "commands/status.h"
#include "commands/commit.h"
#include "commands/print.h"
#include "commands/rm.h"
#include "commands/diff.h"
#include "commands/reset.h"
#include "commands/branch.h"
#include "commands/checkout.h"
#include "commands/merge.h"
#include "commands/cherrypick.h"

/*
#include "commands/rebase.h"
*/

using namespace myvc::commands;

std::unique_ptr<Command> createCommand(std::vector<std::string> args) {
    if(args.size() == 0) return {};
    fs::path cur = fs::current_path();
    std::string subcmd = args.at(0);
    args.erase(args.begin());
    if(subcmd == "init") {
        return std::make_unique<Init>(cur, args);
    } else if(subcmd == "log") {
        return std::make_unique<Log>(cur, args);
    } else if(subcmd == "add") {
        return std::make_unique<Add>(cur, args);
    } else if(subcmd == "status") {
        return std::make_unique<Status>(cur, args);
    } else if(subcmd == "commit") {
        return std::make_unique<Commit>(cur, args);
    } else if(subcmd == "print") {
        return std::make_unique<Print>(cur, args);
    } else if(subcmd == "rm") {
        return std::make_unique<Rm>(cur, args);
    } else if(subcmd == "diff") {
        return std::make_unique<Diff>(cur, args);
    } else if(subcmd == "reset") {
        return std::make_unique<Reset>(cur, args);
    } else if(subcmd == "branch") {
        return std::make_unique<Branch>(cur, args);
    } else if(subcmd == "checkout") {
        return std::make_unique<Checkout>(cur, args);
    } else if(subcmd == "merge") {
        return std::make_unique<Merge>(cur, args);
    } else if(subcmd == "cherry-pick") {
        return std::make_unique<Cherrypick>(cur, args);
    } /*else if(subcmd == "rebase") {
        return std::make_unique<Rebase>(cur, args);
    }*/ else {
        return {};
    }
}

int main(int argc, char *argv[]) {
    std::vector<std::string> args {argv + 1, argv + argc};
    try {
        std::unique_ptr<Command> cmd = createCommand(args);
        if(cmd) {
            cmd->execute();
        } else{
            std::cerr << "usage: myvc subcommand" << std::endl;
        }
    } catch(command_error &e) {
        std::cerr << "error: ";
        std::cerr << std::string {e.what()} << std::endl;
    } catch(std::exception &e) {
        std::cerr << "internal myvc error: ";
        std::cerr << std::string {e.what()} << std::endl;
    }
    return 0;
}
