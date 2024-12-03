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

using namespace myvc::commands;

std::unique_ptr<Command> createCommand(std::vector<std::string> args) {
    if(args.size() == 0) return {};
    fs::path cur = fs::current_path();
    std::string subcmd = args[0];
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
    } else {
        return {};
    }
}

int main(int argc, char *argv[]) {
    std::vector<std::string> args {argv + 1, argv + argc};
    std::unique_ptr<Command> cmd = createCommand(args);
    if(cmd) {
        try {
            cmd->execute();
        } catch(command_error &e) {
            std::cerr << "error: ";
            std::cerr << std::string {e.what()} << std::endl;
        } catch(std::exception &e) {
            std::cerr << "internal myvc error: ";
            std::cerr << std::string {e.what()} << std::endl;
        }
    } else {
        std::cerr << "usage: myvc subcommand" << std::endl;
    }
    return 0;
}
