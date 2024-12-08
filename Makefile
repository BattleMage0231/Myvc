CXX = /usr/local/gcc-14.1.0/bin/g++-14.1.0 -std=c++20 -fmodules-ts -Wall -g

myvc: myvc.o serialize.o blob.o branch.o commit.o head.o index.o store.o tree.o diff.o hash.o treediff.o treebuilder.o repository.o commands/command.o commands/init.o commands/log.o
	$(CXX) *.o commands/*.o -o myvc

# commands/add.o commands/status.o commands/commit.o commands/print.o commands/rm.o commands/diff.o commands/reset.o commands/branch.o commands/checkout.o commands/merge.o commands/cherrypick.o commands/rebase.o

%.o: %.cc
	$(CXX) -MMD -MP -c $< -o $@

-include *.d

clean:
	rm -r *.o commands/*.o commands/*.d *.d myvc
