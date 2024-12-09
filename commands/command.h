#pragma once

#include <filesystem>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include "../repository.h"

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
    fs::path basePath;
    std::vector<std::string> rawArgs;
    std::map<std::string, size_t> flagRules;
    std::map<std::string, std::vector<std::string>> flagArgs;
    bool useRepo;

protected:
    fs::path repoPath;
    std::shared_ptr<Repository> repo;
    std::vector<std::string> args;

    Command(const fs::path &, std::vector<std::string>, bool useRepo = true);

    virtual void printHelpMessage() const = 0;
    virtual void createRules();
    virtual void process() = 0;

    void addFlagRule(std::string, size_t num = 0);
    bool hasFlag(const std::string &) const;
    const std::vector<std::string> &getFlagArgs(const std::string &) const;

    size_t resolveNumber(const std::string &) const;
    Commit resolveSymbol(const std::string &) const;
    Branch &resolveBranch(const std::string &) const; 
    fs::path resolvePath(const std::string &) const;
    Hash resolveHash(const std::string &) const;

    void expectNumberOfArgs(size_t) const;
    void expectExists(const fs::path &) const;
    void expectIsDir(const fs::path &) const;
    void expectIsFile(const fs::path &) const;
    void expectCleanState() const;

public:
    void execute();

    virtual ~Command();
};

}
