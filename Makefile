CXX = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -Wall -g

myvc: myvc.o blob.o branch.o commit.o executor.o hash.o head.o index.o store.o tree.o
	$(CXX) *.o -o myvc

blob.o: blob.cc blob.h serialize.h errors.h
	$(CXX) -c blob.cc

branch.o: branch.cc branch.h serialize.h errors.h
	$(CXX) -c branch.cc

commit.o: commit.cc commit.h serialize.h errors.h
	$(CXX) -c commit.cc

executor.o: executor.cc executor.h
	$(CXX) -c executor.cc

hash.o: hash.cc hash.h serialize.h
	$(CXX) -c hash.cc

head.o: head.cc head.h
	$(CXX) -c head.cc

index.o: index.cc errors.h index.h
	$(CXX) -c index.cc

myvc.o: myvc.cc executor.h
	$(CXX) -c myvc.cc

store.o: store.cc store.h errors.h hash.h
	$(CXX) -c store.cc

tree.o: tree.cc tree.h serialize.h errors.h
	$(CXX) -c tree.cc

blob.h: object.h hash.h

branch.h: stored.h hash.h commit.h

commit.h: object.h hash.h tree.h

errors.h:

executor.h: store.h

hash.h: serialize.h

head.h: stored.h commit.h branch.h

index.h: object.h hash.h tree.h blob.h

object.h: stored.h hash.h

serialize.h:

store.h: object.h tree.h commit.h branch.h hash.h head.h blob.h index.h

stored.h: serialize.h

tree.h: object.h hash.h

clean:
	rm -r *.o myvc
