CXX = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -Wall -g
CXXH = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -c -x c++-system-header

myvc: myvc.o
	$(CXX) -o myvc *.o

myvc.o: repository.o myvc.cc
	$(CXX) -c myvc.cc

repository.o: repository_store.o repository.cc
	$(CXX) -c repository.cc

repository_store.o: commit.o branch.o hash.o head.o tree.o blob.o repository_store.cc
	$(CXX) -c repository_store.cc

branch.o: hash.o writable.o branch.cc
	$(CXX) -c branch.cc

hash.o: hash.cc
	$(CXX) -c hash.cc

writable.o: writable.cc
	$(CXX) -c writable.cc

object.o: hash.o writable.o object.cc
	$(CXX) -c object.cc

commit.o: object.o hash.o commit.cc
	$(CXX) -c commit.cc

tree.o: object.o tree.cc
	$(CXX) -c tree.cc

head.o: hash.o writable.o branch.o commit.o head.cc
	$(CXX) -c head.cc

blob.o: hash.o object.o blob.cc
	$(CXX) -c blob.cc

serialization.o: serialization.cc
	$(CXX) -c serialization.cc

clean:
	rm -r gcm.cache *.o myvc
