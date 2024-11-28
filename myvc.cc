#include "executor.h"

int main(int argc, char *argv[]) {
    std::vector<std::string> args {argv, argv + argc};
    myvc::CommandExecutor c {args};
    c.execute();
    return 0;
}
