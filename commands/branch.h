#pragma once

#include "command.h"

namespace myvc::commands {

class Branch : public Command {
    void printHelpMessage() override;
    void createRules() override;
    void process() override;

public:
    Branch(fs::path, std::vector<std::string>);
};

}
