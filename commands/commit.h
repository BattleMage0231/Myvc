#pragma once

#include "command.h"

namespace myvc::commands {

class Commit : public Command {
    void printHelpMessage() override;
    void createRules() override;
    void process() override;

public:
    Commit(fs::path, std::vector<std::string>);
};

}
