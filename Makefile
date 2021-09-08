CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -Werror -O2 -I$(INCLUDE_DIR)

SRC_DIR=src/
INCLUDE_DIR=include/
BUILD_DIR=build/
TEST_DIR=tests/
BIN_DIR=bin/

FILES=brainfuck exceptions operation_factory operations

SRCS=$(FILES:%=$(SRC_DIR)%.cc)
OBJS=$(FILES:%=$(BUILD_DIR)%.cc.o)

BUILD_DIR_GUARD = @mkdir -p $(BUILD_DIR)
BIN_DIR_GUARD = @mkdir -p $(BIN_DIR)

$(BIN_DIR)main: main.cc $(OBJS)
	$(BIN_DIR_GUARD)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(BIN_DIR)test

$(BIN_DIR)test: $(TEST_DIR)test.cc $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)%.cc.o: $(SRC_DIR)%.cc
	$(BUILD_DIR_GUARD)
	$(CXX) $(CXXFLAGS) $^ -c -o $@

.PHONY: clean test

clean:
	rm -r build
	rm -r bin
