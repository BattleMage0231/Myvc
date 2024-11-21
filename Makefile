CXX = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -Wall -g
CXXH = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -c -x c++-system-header

myvc: object.o commit.o symbol.o branch.o repository.o myvc.o
	$(CXX) -o myvc *.o

dependencies:
	$(CXXH) memory iostream sstream string vector stdexcept

myvc.o: dependencies
	$(CXX) -c myvc.cc

repository.o: dependencies
	$(CXX) -c repository.cc

object.o: dependencies
	$(CXX) -c object.cc

commit.o: dependencies
	$(CXX) -c commit.cc

symbol.o: dependencies
	$(CXX) -c symbol.cc

branch.o: dependencies
	$(CXX) -c branch.cc

clean:
	rm -r gcm.cache *.o myvc
