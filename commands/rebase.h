#pragma once

#include "command.h"

namespace myvc::commands {

class Rebase : public Command {
    void printHelpMessage() const override;
    void createRules() override;
    void process() override;

public:
    inline static const std::string rebaseInfoPath = RepositoryStore::myvcName + "/REBASE_INFO";

    Rebase(fs::path, std::vector<std::string>);
};

}
