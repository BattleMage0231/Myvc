#pragma once

#include "command.h"

namespace myvc::commands {

class Rebase : public Command {
    void printHelpMessage() override;
    void createRules() override;
    void process() override;

    bool rebase_from(const Commit &, const Commit &);

public:
    Rebase(fs::path, std::vector<std::string>);
};

}
