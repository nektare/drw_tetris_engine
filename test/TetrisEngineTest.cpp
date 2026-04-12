#include <gtest/gtest.h>

// The "Cheat": Redefine private as public for the scope of this test file
#define private public
#include "../src/TetrisEngine.hpp"
#undef private

// findRestingY 
TEST(TetrisEngineTest, FindRestingY_EmptyBoard_LandsAtZero) {
    TetrisEngine engine;
    auto piece = engine.PieceDefinitionMap['Q'];
    EXPECT_EQ(engine.findRestingY(piece, 0), 0);
}

TEST(TetrisEngineTest, FindRestingY_BlockedRow_LandsOnTop) {
    TetrisEngine engine;
    engine.board.push_back({true, true, false, false, false, false, false, false, false, false});
    auto piece = engine.PieceDefinitionMap['Q']; // 2 wide
    // col 0-1 are occupied at y=0, Q piece should rest at y=1
    EXPECT_EQ(engine.findRestingY(piece, 0), 1);
}

// placePiece 
TEST(TetrisEngineTest, PlacePiece_QAtOrigin_SetsFourCells) {
    TetrisEngine engine;
    auto piece = engine.PieceDefinitionMap['Q']; // (0,0),(1,0),(0,1),(1,1)
    engine.placePiece(piece, 0, 0);
    EXPECT_TRUE(engine.board[0][0]);
    EXPECT_TRUE(engine.board[0][1]);
    EXPECT_TRUE(engine.board[1][0]);
    EXPECT_TRUE(engine.board[1][1]);
}

TEST(TetrisEngineTest, PlacePiece_GrowsBoardAsNeeded) {
    TetrisEngine engine;
    auto piece = engine.PieceDefinitionMap['I']; // 4 wide, 1 tall
    engine.placePiece(piece, 0, 0);
    EXPECT_EQ(engine.board.size(), 1u);
}

// clearLines 
TEST(TetrisEngineTest, ClearLines_FullRow_IsRemoved) {
    TetrisEngine engine;
    TetrisEngine::Row full;
    full.fill(true);
    engine.board.push_back(full);
    engine.clearLines();
    EXPECT_EQ(engine.board.size(), 0u);
}

TEST(TetrisEngineTest, ClearLines_PartialRow_IsKept) {
    TetrisEngine engine;
    TetrisEngine::Row partial{};
    partial[0] = true;
    engine.board.push_back(partial);
    engine.clearLines();
    EXPECT_EQ(engine.board.size(), 1u);
}

// processLine 
TEST(TetrisEngineTest, ProcessLine_SingleQ_HeightIsTwo) {
    TetrisEngine engine;
    // Suppress stdout for this test
    std::streambuf* orig = std::cout.rdbuf(nullptr);
    engine.processLine("Q0");
    std::cout.rdbuf(orig);
    EXPECT_EQ(engine.board.size(), 2u);
}

TEST(TetrisEngineTest, ProcessLine_TwoQsSameColumn_HeightIsFour) {
    TetrisEngine engine;
    std::streambuf* orig = std::cout.rdbuf(nullptr);
    engine.processLine("Q0,Q0");
    std::cout.rdbuf(orig);
    EXPECT_EQ(engine.board.size(), 4u);
}

// 3 example sequences from the exercise pdf and a few more 
TEST(TetrisEngineTest, Run_DRWInputFile) {
    std::vector<std::string> expected = {
               // Sequence                      Expected
        "1",   // I0,I4,Q8                    →  1   (PDF example 1: bottom row clears)
        "4",   // T1,Z3,I4                    →  4   (PDF example 2: no rows clear)
        "3",   // Q0,I2,I6,I0,I6,I6,Q2,Q4     →  3   (PDF example 3: two rows clear)

        "0",   // Q0,Q2,Q4,Q6,Q8              →  0   (5 Q's fill 10 cols, 2 rows clear)
        "4",   // Q0,Q1                       →  4   (two Q's overlapping, stacking)
        "2",   // L0,J2,L4,J6,Q8              →  2   (mixed tall pieces)
        "4",   // I0,I0,I0,I0                 →  4   (four I's stacked)
        "2",   // T0,T3,T6,T8                 →  2   (four T's side by side)
        "6",   // Z0,S2,Z4,S6,I6              →  6   (staircase of Z/S + I)
        "11",  // L0,L1,L2,L3,L4,L5,L6,L7,L8  → 11  (nine L's packed)
    };

    std::ostringstream captured;
    std::streambuf* orig = std::cout.rdbuf(captured.rdbuf());
    TetrisEngine engine;
   
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
