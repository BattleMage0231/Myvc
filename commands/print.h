#pragma once

#include "command.h"

namespace myvc::commands {

class Print : public Command {
    void printHelpMessage() const override;
    void process() override;

public:
    Print(fs::path, std::vector<std::string>);
};

}
