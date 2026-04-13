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

// StandardEngine 

TEST(StandardEngine, FindRestingY_EmptyBoard_LandsAtZero) {
    StandardEngine engine;
    auto piece = Tetris::PieceTable['Q'];
    EXPECT_EQ(engine.board.findRestingY(piece, 0), 0);
}

TEST(StandardEngine, FindRestingY_BlockedRow_LandsOnTop) {
    StandardEngine engine;
    engine.board.grid.push_back({true, true, false, false, false, false, false, false, false, false});
    auto piece = Tetris::PieceTable['Q'];
    EXPECT_EQ(engine.board.findRestingY(piece, 0), 1);
}

TEST(StandardEngine, PlacePiece_QAtOrigin_SetsFourCells) {
    StandardEngine engine;
    auto piece = Tetris::PieceTable['Q']; // (0,0),(1,0),(0,1),(1,1)
    engine.board.place(piece, 0, 0);
    EXPECT_TRUE(engine.board.grid[0][0]);
    EXPECT_TRUE(engine.board.grid[0][1]);
    EXPECT_TRUE(engine.board.grid[1][0]);
    EXPECT_TRUE(engine.board.grid[1][1]);
}

TEST(StandardEngine, PlacePiece_GrowsBoardAsNeeded) {
    StandardEngine engine;
    auto piece = Tetris::PieceTable['I']; // 4 wide, 1 tall
    engine.board.place(piece, 0, 0);
    EXPECT_EQ(engine.board.grid.size(), 1u);
}

TEST(StandardEngine, ClearLines_FullRow_IsRemoved) {
    StandardEngine engine;
    std::array<bool, 10> full;
    full.fill(true);
    engine.board.grid.push_back(full);
    engine.board.clearFullLines();
    EXPECT_EQ(engine.board.grid.size(), 0u);
}

TEST(StandardEngine, ClearLines_PartialRow_IsKept) {
    StandardEngine engine;
    std::array<bool, 10> partial{};
    partial[0] = true;
    engine.board.grid.push_back(partial);
    engine.board.clearFullLines();
    EXPECT_EQ(engine.board.grid.size(), 1u);
}

TEST(StandardEngine, ProcessLine_SingleQ_HeightIsTwo) {
    StandardEngine engine;
    engine.processLine("Q0");
    EXPECT_EQ(engine.board.grid.size(), 2u);
}

TEST(StandardEngine, ProcessLine_TwoQsSameColumn_HeightIsFour) {
    StandardEngine engine;
    engine.processLine("Q0,Q0");
    EXPECT_EQ(engine.board.grid.size(), 4u);
}

TEST(StandardEngine, Run_DRWInputFile) {
    std::vector<std::string> expected = {
        "1", "4", "3", "0", "4", "2", "4", "2", "6", "11"
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

// BitsetEngine

TEST(BitsetEngine, Collides_EmptyBoard_NoCollision) {
    BitsetEngine engine;
    auto piece = Tetris::BitPieceTable['Q'];
    EXPECT_FALSE(engine.board.collides(piece, 0, 0));
}

TEST(BitsetEngine, Collides_BlockedRow_DetectsCollision) {
    BitsetEngine engine;
    engine.board.rows[0] = 0b11;
    engine.board.height = 1;
    auto piece = Tetris::BitPieceTable['Q'];
    EXPECT_TRUE(engine.board.collides(piece, 0, 0));
}

TEST(BitsetEngine, PlacePiece_QAtOrigin_SetsBits) {
    BitsetEngine engine;
    auto piece = Tetris::BitPieceTable['Q']; // rows: 0b11, 0b11
    engine.board.place(piece, 0, 0);
    EXPECT_EQ(engine.board.rows[0] & 0b11, 0b11u);
    EXPECT_EQ(engine.board.rows[1] & 0b11, 0b11u);
}

TEST(BitsetEngine, PlacePiece_GrowsHeightAsNeeded) {
    BitsetEngine engine;
    auto piece = Tetris::BitPieceTable['I']; // 1 row tall
    engine.board.place(piece, 0, 0);
    EXPECT_EQ(engine.board.height, 1);
}

TEST(BitsetEngine, ClearLines_FullRow_IsRemoved) {
    BitsetEngine engine;
    engine.board.rows[0] = 0x3FF;
    engine.board.height = 1;
    engine.board.clearFullLines();
    EXPECT_EQ(engine.board.height, 0);
}

TEST(BitsetEngine, ClearLines_PartialRow_IsKept) {
    BitsetEngine engine;
    engine.board.rows[0] = 0b11;
    engine.board.height = 1;
    engine.board.clearFullLines();
    EXPECT_EQ(engine.board.height, 1);
}

TEST(BitsetEngine, ProcessLine_SingleQ_HeightIsTwo) {
    BitsetEngine engine;
    engine.processLine("Q0");
    EXPECT_EQ(engine.board.height, 2);
}

TEST(BitsetEngine, ProcessLine_TwoQsSameColumn_HeightIsFour) {
    BitsetEngine engine;
    engine.processLine("Q0,Q0");
    EXPECT_EQ(engine.board.height, 4);
}

TEST(BitsetEngine, Run_DRWInputFile) {
    std::vector<std::string> expected = {
        "1", "4", "3", "0", "4", "2", "4", "2", "6", "11"
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
