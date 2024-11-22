import <iostream>;
import <memory>;
import <vector>;
import <stdexcept>;

import repository;

using namespace myvc;
using namespace std;

int main(int argc, char *argv[]) {
    cout << "Hello, World" << endl;
    /*
    unique_ptr<Tree> t1 = make_shared<Tree>(), t2 = make_shared<Tree>();
    shared_ptr<Commit> c1 = make_shared<Commit>(t1);
    shared_ptr<Commit> c2 = make_shared<Commit>(t2);
    Branch b1 {"branch 1", c1}, b2 {"branch 2", c2};
    unique_ptr<Symbol> a = make_unique<Branch>("head branch askdahs", c1);
    Repository repo {std::move(a), vector<Branch> {a, b1, b2}}; // todo figure this out.. what should HEAD be?
    const Symbol &head = repo.getHead();
    cout << head.getName() << endl;
    try {
        const Branch &b = static_cast<const Branch &>(head);
        const Commit &c = *b;
        c.getParent();
    } catch(logic_error &e) {
        cout << "asdasd" << endl;
        cerr << e.what() << endl;
    }
    cout << "asdasd" << endl;
    */
    return 0;
}
