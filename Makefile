CXX = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -Wall -g

myvc: myvc.o blob.o branch.o commit.o hash.o head.o index.o store.o tree.o diff.o treediff.o commands/command.o commands/init.o commands/log.o commands/add.o commands/status.o
	$(CXX) *.o commands/*.o -o myvc

%.o: %.cc
	$(CXX) -MMD -MP -c $< -o $@

-include *.d

clean:
	rm -r *.o commands/*.o commands/*.d *.d myvc
