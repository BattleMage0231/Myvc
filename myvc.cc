#include <filesystem>
#include "executor.h"

int main(int argc, char *argv[]) {
    std::vector<std::string> args {argv + 1, argv + argc};
    CommandExecutor c {std::filesystem::current_path(), args};
    c.execute();
    return 0;
}
