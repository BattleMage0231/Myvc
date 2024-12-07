#pragma once

#include <sstream>
#include <string>
#include <exception>

namespace myvc {

#define THROW(reason) std::throw_with_nested(myvc::error(reason, __FILE__, __LINE__));

class error : public std::exception {
    std::string message;

public:
    error(std::string reason, std::string file, size_t line) {
        std::stringstream ss;
        ss << reason << " (in " << file << ", line " << line << ") ";
        message = ss.str();
    }

    const char *what() const noexcept {
        return message.c_str();
    }
};

class not_implemented : public std::exception {
public:
    const char *what() const noexcept {
        return "Not implemented";
    }
};

}
