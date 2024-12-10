#pragma once

#include "command.h"

namespace myvc::commands {

class Cherrypick : public Command {
    void printHelpMessage() const override;
    void process() override;

public:
    Cherrypick(fs::path, std::vector<std::string>);
};

}
