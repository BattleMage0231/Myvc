export module repository;

export import <filesystem>;
import <memory>;
import <vector>;

import repository_store;

namespace myvc {

namespace fs = std::filesystem;

export class Repository {
    RepositoryStore store;

public:
    explicit Repository(const fs::path &path, bool create=false) : store {path, create} {} 
};

}
