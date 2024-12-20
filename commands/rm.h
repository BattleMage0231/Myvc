#pragma once

#include "command.h"

namespace myvc::commands {

class Rm : public Command {
    void printHelpMessage() const override;
    void createRules() override;
    void process() override;

public:
    Rm(fs::path, std::vector<std::string>);
};

}
