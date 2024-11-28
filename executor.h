#pragma once

#include <vector>
#include <string>
#include <utility>
#include <map>
#include <vector>
#include <memory>
#include "store.h"

using namespace myvc;

enum class Subcommand {
    None, Init, Status
};

class CommandExecutor {
    std::vector<std::string> args;
    fs::path path;
    std::shared_ptr<RepositoryStore> store;
    Subcommand subcommand;
    std::map<std::string, size_t> flagRules;
    std::map<std::string, std::vector<std::string>> flagArgs;
    std::vector<std::string> nonFlagArgs;

    void parseSubcommand();
    void parseArgs();
    void printHelpMessage() const;

    void init();
    void status();

public:
    explicit CommandExecutor(fs::path, std::vector<std::string> args = {});

    void execute();
};

