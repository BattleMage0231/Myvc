#include "command.h"

using namespace myvc::commands;

Command::Command(fs::path path, std::vector<std::string> rawArgs, bool useStore)
    : rawArgs {std::move(rawArgs)}, useStore {useStore}, path {std::move(path)} {}

void Command::parseArgs() {
    for(size_t i = 0; i < rawArgs.size();) {
        if(flagRules.find(rawArgs[i]) != flagRules.end()) {
            size_t needed = flagRules[rawArgs[i++]];
            if(rawArgs.size() - i < needed) throw std::runtime_error {""};
            for(; needed > 0; --needed, ++i) {
                flagArgs[rawArgs[i]].emplace_back(rawArgs[i]);
            }
        } else {
            args.push_back(rawArgs[i++]);
        }
    }
}

void Command::execute() {
    flagRules["--help"] = 0;
    flagRules["-h"] = 0;
    createRules();
    parseArgs();
    if(flagArgs.find("-h") != flagArgs.end() || flagArgs.find("--help") != flagArgs.end()) {
        printHelpMessage();
    } else {
        if(useStore) store = std::make_shared<RepositoryStore>(path);
        process();
    }
}
