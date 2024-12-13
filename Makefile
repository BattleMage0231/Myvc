CXX = g++-14.2.0 -std=c++20 -Wall -O3
FLAGS =
DEP = serialize.o blob.o branch.o commit.o head.o index.o store.o tree.o diff.o hash.o treediff.o treebuilder.o repository.o 

myvc: $(DEP) myvc.o commands/command.o commands/init.o commands/log.o commands/add.o commands/status.o commands/commit.o commands/print.o commands/rm.o commands/diff.o commands/reset.o commands/branch.o commands/checkout.o commands/merge.o commands/cherrypick.o commands/rebase.o
	$(CXX) myvc.o $(DEP) commands/*.o -o myvc

convert: $(DEP) myvc-convert.o
	$(CXX) myvc-convert.o $(DEP) -o myvc-convert -lgit2

%.o: %.cc
	$(CXX) $(FLAGS) -MMD -MP -c $< -o $@

-include *.d

debug:
	$(MAKE) FLAGS=-DMYVCDEBUG myvc

clean:
	rm -rf *.o commands/*.o commands/*.d *.d myvc
