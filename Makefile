CXX=g++
CXXFLAGS=--std=c++17 -Wall -Wextra -Werror -O2

all: brainfuck tests/test

brainfuck: brainfuck.cc bf.o
	$(CXX) $(CXXFLAGS) $^ -o $@

tests/test: tests/test.cc bf.o
	$(CXX) $(CXXFLAGS) $^ -o $@

bf.o: bf.cc bf.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm bf.o
	rm tests/test
	rm brainfuck
