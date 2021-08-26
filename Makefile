CXX=g++
CXXFLAGS=--std=c++17 -Wall -Wextra -Werror -O2

brainfuck: brainfuck.cc
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	rm brainfuck
