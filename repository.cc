export module repository;

import <memory>;
import <vector>;

export import symbol;
export import commit;
export import branch;

namespace myvc {

using std::vector, std::unique_ptr;

export class Repository {
    unique_ptr<Symbol> head;
    vector<Branch> branches;

public:
    Repository(unique_ptr<Symbol> head, const vector<Branch>& branches) : 
        head {std::move(head)}, branches {branches} {}

    const Symbol &getHead() const {
        return *head;
    }
};

}
