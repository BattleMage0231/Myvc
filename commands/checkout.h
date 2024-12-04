#pragma once

#include "command.h"

namespace myvc::commands {

class Checkout : public Command {
    void printHelpMessage() override;
    void process() override;

public:
    Checkout(fs::path, std::vector<std::string>);
};

}
