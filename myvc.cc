import <iostream>;

import repository;

using namespace myvc;
using std::cout, std::endl;

int main(int argc, char *argv[]) {
    cout << "Hello, World" << endl;
    Commit c;
    Repository repo {c};
    return 0;
}
