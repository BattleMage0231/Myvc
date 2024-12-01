#pragma once

#include "command.h"

namespace myvc::commands {

class Log : public Command {
    void printHelpMessage() override;
    void createRules() override;
    void process() override;

public:
    Log(fs::path, std::vector<std::string>);
};

}
