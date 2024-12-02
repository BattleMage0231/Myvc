#pragma once

#include "command.h"

namespace myvc::commands {

class Add : public Command {
    void printHelpMessage() override;
    void createRules() override;
    void process() override;

    void addDir(Index &, const fs::path &, const Tree &);
    void addDirRecursive(Index &, const fs::path &, const Tree &);

public:
    Add(fs::path, std::vector<std::string>);
};

}
