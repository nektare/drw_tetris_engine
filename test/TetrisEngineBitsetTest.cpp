#include <gtest/gtest.h>

#define private public
#include "../src/TetrisEngineBitset.hpp"
#undef private

// collides (exercises findRestingY logic)
TEST(TetrisEngineBitset, Collides_EmptyBoard_NoCollision) {
    TetrisEngineBitset engine;
    auto piece = Tetris::PieceTable['Q'];
    EXPECT_FALSE(engine.board.collides(piece, 0, 0));
}

TEST(TetrisEngineBitset, Collides_BlockedRow_DetectsCollision) {
    TetrisEngineBitset engine;
    engine.board.rows[0] = 0b11; // cols 0,1 occupied at row 0
    engine.board.height = 1;
    auto piece = Tetris::PieceTable['Q']; // 2 wide
    EXPECT_TRUE(engine.board.collides(piece, 0, 0));
}

// place
TEST(TetrisEngineBitset, PlacePiece_QAtOrigin_SetsBits) {
    TetrisEngineBitset engine;
    auto piece = Tetris::PieceTable['Q']; // rows: 0b11, 0b11
    engine.board.place(piece, 0, 0);
    EXPECT_EQ(engine.board.rows[0] & 0b11, 0b11u);
    EXPECT_EQ(engine.board.rows[1] & 0b11, 0b11u);
}

TEST(TetrisEngineBitset, PlacePiece_GrowsHeightAsNeeded) {
    TetrisEngineBitset engine;
    auto piece = Tetris::PieceTable['I']; // 1 row tall
    engine.board.place(piece, 0, 0);
    EXPECT_EQ(engine.board.height, 1);
}

// clearFullLines
TEST(TetrisEngineBitset, ClearLines_FullRow_IsRemoved) {
    TetrisEngineBitset engine;
    engine.board.rows[0] = 0x3FF; // all 10 bits set
    engine.board.height = 1;
    engine.board.clearFullLines();
    EXPECT_EQ(engine.board.height, 0);
}

TEST(TetrisEngineBitset, ClearLines_PartialRow_IsKept) {
    TetrisEngineBitset engine;
    engine.board.rows[0] = 0b11; // partial row
    engine.board.height = 1;
    engine.board.clearFullLines();
    EXPECT_EQ(engine.board.height, 1);
}

// processLine
TEST(TetrisEngineBitset, ProcessLine_SingleQ_HeightIsTwo) {
    TetrisEngineBitset engine;
    engine.processLine("Q0");
    EXPECT_EQ(engine.board.height, 2);
}

TEST(TetrisEngineBitset, ProcessLine_TwoQsSameColumn_HeightIsFour) {
    TetrisEngineBitset engine;
    engine.processLine("Q0,Q0");
    EXPECT_EQ(engine.board.height, 4);
}

// run
TEST(TetrisEngineBitset, Run_DRWInputFile) {
    std::vector<std::string> expected = {
               // Sequence                          Expected
        "1",   // I0,I4,Q8                    →  1   (PDF example 1: bottom row clears)
        "4",   // T1,Z3,I4                    →  4   (PDF example 2: no rows clear)
        "3",   // Q0,I2,I6,I0,I6,I6,Q2,Q4    →  3   (PDF example 3: two rows clear)

        "0",   // Q0,Q2,Q4,Q6,Q8             →  0   (5 Q's fill 10 cols, 2 rows clear)
        "4",   // Q0,Q1                       →  4   (two Q's overlapping, stacking)
        "2",   // L0,J2,L4,J6,Q8             →  2   (mixed tall pieces)
        "4",   // I0,I0,I0,I0                →  4   (four I's stacked)
        "2",   // T0,T3,T6,T8                →  2   (four T's side by side)
        "6",   // Z0,S2,Z4,S6,I6             →  6   (staircase of Z/S + I)
        "11",  // L0,L1,L2,L3,L4,L5,L6,L7,L8 → 11  (nine L's packed)
    };

    std::ostringstream captured;
    std::streambuf* orig = std::cout.rdbuf(captured.rdbuf());
    TetrisEngineBitset engine;
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
        size_t lb = line.find('[');
        size_t rb = line.find(']');
        if (lb == std::string::npos || rb == std::string::npos) continue;
        std::string height = line.substr(lb + 1, rb - lb - 1);
        EXPECT_EQ(height, expected[i]) << "Line " << i;
        ++i;
    }
    EXPECT_EQ(i, (int)expected.size());
}
