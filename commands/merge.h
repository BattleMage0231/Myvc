#pragma once

#include "command.h"

namespace myvc::commands {

class Merge : public Command {
    void printHelpMessage() override;
    void process() override;

public:
    Merge(fs::path, std::vector<std::string>);
};

}
