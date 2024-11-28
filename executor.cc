#include <stdexcept>
#include <memory>
#include "executor.h"

using namespace myvc;

CommandExecutor::CommandExecutor(fs::path path, std::vector<std::string> args) 
    : args {std::move(args)}, path {std::move(path)}
{
    for(auto x : args) std::cout << x << std::endl;
}

void CommandExecutor::parseSubcommand() {
    flagRules["--help"] = 0;
    flagRules["-h"] = 0;
    if(args.size() == 0) {
        subcommand = Subcommand::None;
    } else if(args[0] == "init") {
        flagRules["--testflag"] = 1;
        subcommand = Subcommand::Init;
    } else if(args[0] == "status") {
        subcommand = Subcommand::Status;
    } else {
        subcommand = Subcommand::None;
    }
}

void CommandExecutor::parseArgs() {
    for(size_t i = 1; i < args.size(); ++i) {
        if(flagRules.find(args[i]) != flagRules.end()) {
            size_t needed = flagRules[args[i++]];
            if(args.size() - i < needed) throw std::runtime_error {""};
            flagArgs[args[i]] = std::vector<std::string> {args.begin() + i, args.begin() + i + needed};
            i += needed;
        } else {
            nonFlagArgs.push_back(args[i]);
        }
    }
}

void CommandExecutor::printHelpMessage() const {
    switch(subcommand) {
        case Subcommand::None:
            std::cout << "usage: myvc subcommand [args]" << std::endl;
            break;
        case Subcommand::Init:
            std::cout << "usage: myvc init [--testflag x]" << std::endl;
            break;
        case Subcommand::Status:
            std::cout << "usage: myvc status" << std::endl;
            break;
    }
}

void CommandExecutor::execute() {
    parseSubcommand();
    try {
        parseArgs();
    } catch(std::runtime_error &e) {
        std::cout << "error: flag requires an argument" << std::endl;
        return;
    }
    bool help = (flagArgs.find("-h") != flagArgs.end()) || (flagArgs.find("--h") != flagArgs.end());
    if(subcommand == Subcommand::None || help) {
        printHelpMessage();
        return;
    } else if(subcommand == Subcommand::Init) {
        init();
    } else {
        store = std::make_shared<RepositoryStore>(path);
        if(subcommand == Subcommand::Status) status();
    }
}

void CommandExecutor::init() {
    std::cout << "init called" << std::endl;
}

void CommandExecutor::status() {
    std::cout << "status called" << std::endl;
}
