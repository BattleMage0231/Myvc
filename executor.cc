#include <stdexcept>
#include <memory>
#include "executor.h"
#include "hash.h"
#include "serialize.h"

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
    } else if(args[0] == "test") {
        subcommand = Subcommand::TESTING;
    } else {
        subcommand = Subcommand::None;
    }
}

void CommandExecutor::parseArgs() {
    for(size_t i = 1; i < args.size();) {
        if(flagRules.find(args[i]) != flagRules.end()) {
            size_t needed = flagRules[args[i++]];
            if(args.size() - i < needed) throw std::runtime_error {""};
            for(; needed > 0; --needed, ++i) {
                flagArgs[args[i]].emplace_back(args[i]);
            }
        } else {
            nonFlagArgs.push_back(args[i++]);
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
        case Subcommand::TESTING:
            std::cout << "Bad" << std::endl;
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
        else if(subcommand == Subcommand::TESTING) {
            testing();
        }
    }
}

void CommandExecutor::init() {
    std::cout << "init called" << std::endl;
}

void CommandExecutor::status() {
    std::cout << "status called" << std::endl;
}

struct StrWrapper : public Serializable {
    std::string s;
    StrWrapper(std::string s) : s {std::move(s)} {}
    void write(std::ostream &out) const override {
        out.write(s.data(), s.size());
    }
    void read(std::istream &in) override {}
};

void CommandExecutor::testing() {
    //std::string str = nonFlagArgs.at(0);
    Blob b {{'a'}};
    std::cout << static_cast<std::string>(b.getHash()) << std::endl;
}
