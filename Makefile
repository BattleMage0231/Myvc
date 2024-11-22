CXX = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -Wall -g
CXXH = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -c -x c++-system-header

myvc: hash.o writable.o object.o tree.o commit.o branch.o repository_store.o repository.o myvc.o
	$(CXX) -o myvc *.o

dependencies:
	$(CXXH) memory iostream sstream string vector stdexcept chrono filesystem fstream map variant

myvc.o: dependencies
	$(CXX) -c myvc.cc

hash.o: dependencies
	$(CXX) -c hash.cc

writable.o: dependencies
	$(CXX) -c writable.cc

repository.o: dependencies
	$(CXX) -c repository.cc

object.o: dependencies
	$(CXX) -c object.cc

commit.o: dependencies
	$(CXX) -c commit.cc

branch.o: dependencies
	$(CXX) -c branch.cc

tree.o: dependencies
	$(CXX) -c tree.cc

repository_store.o: dependencies
	$(CXX) -c repository_store.cc

clean:
	rm -r gcm.cache *.o myvc
