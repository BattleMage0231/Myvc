#pragma once

#include "command.h"

namespace myvc::commands {

class Status : public Command {
    void printHelpMessage() const override;
    void process() override;

public:
    Status(fs::path, std::vector<std::string>);
};

}
