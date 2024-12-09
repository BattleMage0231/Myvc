#pragma once

#include "command.h"

namespace myvc::commands {

class Reset : public Command {
    void printHelpMessage() const override;
    void createRules() override;
    void process() override;

public:
    Reset(fs::path, std::vector<std::string>);
};

}
