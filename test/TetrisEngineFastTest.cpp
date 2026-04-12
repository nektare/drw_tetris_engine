#include <gtest/gtest.h>

#define private public
#include "../src/TetrisEngineFast.hpp"
#undef private

// findRestingY
TEST(TetrisEngineFastTest, FindRestingY_EmptyBoard_LandsAtZero) {
    TetrisEngineFast engine;
    auto p = engine.getPiece('Q');
    EXPECT_EQ(engine.findRestingY(p, 0), 0);
}

TEST(TetrisEngineFastTest, FindRestingY_BlockedRow_LandsOnTop) {
    TetrisEngineFast engine;
    engine.board[0] = 0b11; // cols 0,1 occupied at row 0
    engine.currentHeight = 1;
    auto p = engine.getPiece('Q');
    EXPECT_EQ(engine.findRestingY(p, 0), 1);
}

// placePiece
TEST(TetrisEngineFastTest, PlacePiece_QAtOrigin_SetsFourCells) {
    TetrisEngineFast engine;
    auto p = engine.getPiece('Q'); // rows: 0b11, 0b11
    engine.placePiece(p, 0, 0);
    EXPECT_EQ(engine.board[0] & 0b11, 0b11u);
    EXPECT_EQ(engine.board[1] & 0b11, 0b11u);
}

TEST(TetrisEngineFastTest, PlacePiece_GrowsHeightAsNeeded) {
    TetrisEngineFast engine;
    auto p = engine.getPiece('I'); // 1 row tall
    engine.placePiece(p, 0, 0);
    EXPECT_EQ(engine.currentHeight, 1);
}

// clearLines
TEST(TetrisEngineFastTest, ClearLines_FullRow_IsRemoved) {
    TetrisEngineFast engine;
    engine.board[0] = 0x3FF; // all 10 bits set
    engine.currentHeight = 1;
    engine.clearLines();
    EXPECT_EQ(engine.currentHeight, 0);
}

TEST(TetrisEngineFastTest, ClearLines_PartialRow_IsKept) {
    TetrisEngineFast engine;
    engine.board[0] = 0b11; // partial row
    engine.currentHeight = 1;
    engine.clearLines();
    EXPECT_EQ(engine.currentHeight, 1);
}

// processLine
TEST(TetrisEngineFastTest, ProcessLine_SingleQ_HeightIsTwo) {
    TetrisEngineFast engine;
    std::streambuf* orig = std::cout.rdbuf(nullptr);
    engine.processLine("Q0");
    std::cout.rdbuf(orig);
    EXPECT_EQ(engine.currentHeight, 2);
}

TEST(TetrisEngineFastTest, ProcessLine_TwoQsSameColumn_HeightIsFour) {
    TetrisEngineFast engine;
    std::streambuf* orig = std::cout.rdbuf(nullptr);
    engine.processLine("Q0,Q0");
    std::cout.rdbuf(orig);
    EXPECT_EQ(engine.currentHeight, 4);
}

// run
TEST(TetrisEngineFastTest, Run_DRWInputFile) {
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
    TetrisEngineFast engine;
    engine.run("test/drw_test_input.txt");
    std::cout.rdbuf(orig);

    std::istringstream ss(captured.str());
    std::string line;
    int i = 0;
    while (std::getline(ss, line)) {
        EXPECT_EQ(line, expected[i]) << "Line " << i;
        ++i;
    }
    EXPECT_EQ(i, (int)expected.size());
}
