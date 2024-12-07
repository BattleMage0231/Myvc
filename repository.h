#pragma once

#include "store.h"

namespace myvc {

class Repository {
    fs::path path;
    RepositoryStore store;

public:
    explicit Repository(fs::path);
};

}
