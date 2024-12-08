#pragma once

#include "command.h"

namespace myvc::commands {

class Commit : public Command {
    void printHelpMessage() const override;
    void createRules() override;
    void process() override;

public:
    static const std::string editMessagePath;
    static const std::string editorName;

    Commit(fs::path, std::vector<std::string>);
};

}
