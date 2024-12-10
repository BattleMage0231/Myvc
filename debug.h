#pragma once

#include <iostream>
#include <limits>
#include "errors.h"

namespace myvc {

inline void debug_log(const std::string &msg) {
#ifdef MYVCDEBUG
    std::cerr << "INFO: " << msg << std::endl;
#endif
}

inline void debug_prompt(const std::string &question) {
#ifdef MYVCDEBUG
    std::cin.ignore(std::numeric_limits<std::streamsize>::max());
    std::cerr << "Y/N: " << question << std::endl;
    std::string r; std::cin >> r;
    if(r != "y") {
        THROW("prompt failed");
    }
#endif
}

}
