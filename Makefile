CXX      = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

SRC_DIR  = src
TEST_DIR = test
BIN_DIR  = build

TEST_TARGET  = $(BIN_DIR)/TetrisEngineTest
PERF_TARGET  = $(BIN_DIR)/TetrisEnginePerfTest
TEST_OUTPUT  = test/drw_test_output.txt

GTEST_FLAGS = $(shell pkg-config --cflags --libs gtest_main)

.PHONY: all test perf clean

all: $(BIN_DIR) $(TEST_TARGET)

test: $(BIN_DIR) $(TEST_TARGET)
	./$(TEST_TARGET) 2>&1 | tee $(TEST_OUTPUT)

perf: $(BIN_DIR) $(PERF_TARGET)
	./$(PERF_TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TEST_TARGET): $(TEST_DIR)/TetrisEngineTest.cpp \
                $(SRC_DIR)/TetrisEngine.hpp \
                $(SRC_DIR)/TetrisTypes.hpp \
                $(SRC_DIR)/TetrisTypesBitset.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(GTEST_FLAGS)

$(PERF_TARGET): $(TEST_DIR)/TetrisEnginePerfTest.cpp \
                $(SRC_DIR)/TetrisEngine.hpp \
                $(SRC_DIR)/TetrisTypes.hpp \
                $(SRC_DIR)/TetrisTypesBitset.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(BIN_DIR)
	truncate -s 0 $(TEST_OUTPUT)
