#pragma once

#include <vector>
#include <string>
#include "store.h"

namespace myvc {

class CommandExecutor {
    std::vector<std::string> args;

public:
    explicit CommandExecutor(std::vector<std::string> args = {});

    void execute();
};

}
