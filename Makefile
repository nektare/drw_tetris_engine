CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRC_DIR  = src
TEST_DIR = test
BIN_DIR  = build

TARGET      = $(BIN_DIR)/TetrisEngine
TEST_TARGET          = $(BIN_DIR)/TetrisEngineTest
FAST_TEST_TARGET     = $(BIN_DIR)/TetrisEngineFastTest
BITSET_TEST_TARGET   = $(BIN_DIR)/TetrisEngineBitsetTest

GTEST_FLAGS = $(shell pkg-config --cflags --libs gtest_main)

.PHONY: all test fast-test clean

all: $(BIN_DIR) $(TARGET)

test: $(BIN_DIR) $(TEST_TARGET) $(FAST_TEST_TARGET) $(BITSET_TEST_TARGET)
	./$(TEST_TARGET)
	./$(FAST_TEST_TARGET)
	./$(BITSET_TEST_TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(SRC_DIR)/TetrisEngine.hpp
	$(CXX) $(CXXFLAGS) -x c++ -o $@ $<

$(TEST_TARGET): $(TEST_DIR)/TetrisEngineTest.cpp $(SRC_DIR)/TetrisEngine.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(GTEST_FLAGS)

$(FAST_TEST_TARGET): $(TEST_DIR)/TetrisEngineFastTest.cpp $(SRC_DIR)/TetrisEngineFast.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(GTEST_FLAGS)

$(BITSET_TEST_TARGET): $(TEST_DIR)/TetrisEngineBitsetTest.cpp $(SRC_DIR)/TetrisEngineBitset.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(GTEST_FLAGS)

clean:
	rm -rf $(BIN_DIR)
