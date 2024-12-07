#include "repository.h"

using namespace myvc;

Repository::Repository(fs::path path) : path {path}, store {path} {}
