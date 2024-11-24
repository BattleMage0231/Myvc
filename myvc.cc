#include <iostream>

import repository;

using namespace myvc;
using namespace std;

int main(int argc, char *argv[]) {
    Repository r {"./tests/repo1", false};
    r.testFunction();
    cout << "Hello World" << endl;
    return 0;
}
