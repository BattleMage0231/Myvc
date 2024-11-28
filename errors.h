#pragma once

#include <string>
#include <exception>

namespace myvc {

class not_found : public std::exception {
    std::string resource;

public:
    not_found(std::string resource) : resource {std::move(resource)} {}

    const char *what() const noexcept {
        return resource.c_str();
    }
};

class not_implemented : public std::exception {
public:
    const char *what() const noexcept {
        return "Not implemented";
    }
};

}
