#pragma once

#include <filesystem>
#include "debug.h"
#include "store.h"

namespace myvc {

namespace fs = std::filesystem;

namespace fileops {

// wrappers around std::filesystem remove operations to protect my files during testing

inline void remove_all(const fs::path &path) {
    debug_prompt("remove all at " + static_cast<std::string>(path));
    fs::remove_all(path);
}

inline void remove(const fs::path &path) {
    debug_prompt("remove at " + static_cast<std::string>(path));
    fs::remove(path);
}

}

}
