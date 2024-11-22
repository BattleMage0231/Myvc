export module repository;

export import <filesystem>;
import <memory>;
import <vector>;

export import symbol;
export import commit;
export import branch;

namespace myvc {

namespace fs = std::filesystem;
using std::vector, std::unique_ptr;

export class Repository {
    RepositoryStore store;

public:
    explicit Repository(const fs::path &path, bool create=false) : store {path, create} {} 
};

}
