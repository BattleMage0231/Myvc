export module repository;

export import commit;

namespace myvc {

export class Repository {
    Commit head;

public:
    Repository(const Commit &head) : head {head} {}

    const Commit &getHead() const {
        return head;
    }
};

}
