#pragma once

#include <filesystem>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include "../store.h"
#include "../commit.h"

namespace myvc::commands {

namespace fs = std::filesystem;

class command_error : public std::exception {
    std::string reason;

public:
    command_error(std::string reason) : reason {std::move(reason)} {}

    const char *what() const noexcept {
        return reason.c_str();
    }
};

class Command {
    std::vector<std::string> rawArgs;
    bool useStore;

    void parseArgs();

protected:
    fs::path path;
    std::shared_ptr<RepositoryStore> store;
    std::map<std::string, size_t> flagRules;
    std::map<std::string, std::vector<std::string>> flagArgs;
    std::vector<std::string> args;

    Command(fs::path, std::vector<std::string>, bool useStore = true);

    virtual void printHelpMessage() = 0;
    virtual void createRules() {};
    virtual void process() = 0;

    size_t resolveNumber(const std::string &) const;
    Commit resolveSymbol(const std::string &) const;
    Head resolveHead() const;

public:
    void execute();

    virtual ~Command() {}
};

}
