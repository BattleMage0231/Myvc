#pragma once

#include "command.h"

namespace myvc::commands {

class Add : public Command {
    void printHelpMessage() override;
    void createRules() override;
    void process() override;

public:
    Add(fs::path, std::vector<std::string>);
};

}
