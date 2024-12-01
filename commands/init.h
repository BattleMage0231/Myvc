#pragma once

#include "command.h"

namespace myvc::commands {

class Init : public Command {
    void printHelpMessage() override;
    void process() override;

public:
    Init(fs::path, std::vector<std::string>);
};

}