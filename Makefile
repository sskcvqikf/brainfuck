CXX=g++
CXXFLAGS=--std=c++17 -Wall -Wextra -Werror -O2

all: brainfuck tests/test

brainfuck: brainfuck.cc bf.o
	$(CXX) $(CXXFLAGS) $< bf.o -o $@

tests/test: tests/test.cc bf.o
	$(CXX) $(CXXFLAGS) $< bf.o -o $@

bf.o: bf.cc bf.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm brainfuck
