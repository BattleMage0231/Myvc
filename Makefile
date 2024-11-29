CXX = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -Wall -g

myvc: myvc.o blob.o branch.o commit.o executor.o hash.o head.o index.o store.o tree.o
	$(CXX) *.o -o myvc

%.o: %.cc
	$(CXX) -MMD -MP -c $< -o $@

-include *.d

clean:
	rm -r *.o *.d myvc
