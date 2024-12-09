#pragma once

#include "command.h"

namespace myvc::commands {

class Merge : public Command {
    void printHelpMessage() const override;
    void createRules() override;
    void process() override;

public:
    inline static const std::string mergeInfoPath = RepositoryStore::myvcName + "/MERGE_INFO";

    Merge(fs::path, std::vector<std::string>);
};

}
