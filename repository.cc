module;

#include <iostream>
#include <filesystem>
#include <memory>
#include <vector>

export module repository;

import repository_store;

namespace myvc {

namespace fs = std::filesystem;

export class Repository {
    RepositoryStore store;

public:
    explicit Repository(fs::path path, bool create=false) : store {std::move(path), create} {} 

    // todo remove this
    void testFunction() {
        try {
            const Commit &m = store.getCommit(Hash {"temporary_hash"});
            std::cout << "Fetched: " << m << std::endl;
        } catch(...) {
            Commit m {Hash {"tree_hash_placeholder"}};
            store.createCommit(m);
            std::cout << "Stored:" << m;
        }
    }
};

}
