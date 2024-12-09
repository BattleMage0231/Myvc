#pragma once

#include "command.h"

namespace myvc::commands {

class Commit : public Command {
    void printHelpMessage() const override;
    void createRules() override;
    void process() override;

public:
    inline static const std::string editMessagePath = RepositoryStore::myvcName + "/COMMIT_EDITMSG";
    static constexpr std::string editorName = "vim";

    Commit(fs::path, std::vector<std::string>);
};

}
