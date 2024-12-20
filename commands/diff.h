#pragma once

#include "command.h"

namespace myvc::commands {

class Diff : public Command {
    void printHelpMessage() const override;
    void createRules() override;
    void process() override;

public:
    Diff(fs::path, std::vector<std::string>);
};

}
