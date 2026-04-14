#include <gtest/gtest.h>
#include <sstream>
#include <algorithm>

#define private public
#include "../src/TetrisEngine.hpp"
#include "../src/TetrisTypes.hpp"
#include "../src/TetrisTypesBitset.hpp"
#undef private

using StandardEngine = TetrisEngine<
    Tetris::Board,
    Tetris::Piece,
    Tetris::PieceTable
>;

using BitsetEngine = TetrisEngine<
    Tetris::BitBoard,
    Tetris::BitPiece,
    Tetris::BitPieceTable
>;


// two full tests - first for standard and then for bitset engine

TEST(StandardEngine, Run_DRWInputFile) {
    std::vector<std::string> expected = {
        "2", "4", "0", "2", "4", "1", "0", "2", "2", "2",
        "1", "1", "4", "3", "1", "2", "1", "8", "8", "0", "3"
    };

    std::ostringstream captured;
    std::streambuf* orig = std::cout.rdbuf(captured.rdbuf());
    StandardEngine engine;
    engine.run("test/drw_test_input.txt");
    std::cout.rdbuf(orig);
    std::cout << captured.str();

    std::string output = captured.str();
    std::replace(output.begin(), output.end(), '\r', '\n');
    std::istringstream ss(output);
    std::string line;
    int i = 0;
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        size_t lb = line.find('['), rb = line.find(']');
        if (lb == std::string::npos || rb == std::string::npos) continue;
        EXPECT_EQ(line.substr(lb + 1, rb - lb - 1), expected[i]) << "Line " << i;
        ++i;
    }
    EXPECT_EQ(i, (int)expected.size());
}

TEST(BitsetEngine, Run_DRWInputFile) {
    std::vector<std::string> expected = {
        "2", "4", "0", "2", "4", "1", "0", "2", "2", "2",
        "1", "1", "4", "3", "1", "2", "1", "8", "8", "0", "3"
    };

    std::ostringstream captured;
    std::streambuf* orig = std::cout.rdbuf(captured.rdbuf());
    BitsetEngine engine;
    engine.run("test/drw_test_input.txt");
    std::cout.rdbuf(orig);
    std::cout << captured.str();

    std::string output = captured.str();
    std::replace(output.begin(), output.end(), '\r', '\n');
    std::istringstream ss(output);
    std::string line;
    int i = 0;
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        size_t lb = line.find('['), rb = line.find(']');
        if (lb == std::string::npos || rb == std::string::npos) continue;
        EXPECT_EQ(line.substr(lb + 1, rb - lb - 1), expected[i]) << "Line " << i;
        ++i;
    }
    EXPECT_EQ(i, (int)expected.size());
}

