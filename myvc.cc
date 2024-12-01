#include <filesystem>
#include <memory>
#include <iostream>
#include "commands/command.h"
#include "commands/init.h"

using namespace myvc::commands;

std::unique_ptr<Command> createCommand(std::vector<std::string> args) {
    if(args.size() == 0) return {};
    fs::path cur = fs::current_path();
    std::string subcmd = args[0];
    args.erase(args.begin());
    if(subcmd == "init") {
        return std::make_unique<Init>(cur, args);
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
        }
    } else {
        std::cerr << "usage: myvc subcommand" << std::endl;
    }
    return 0;
}
