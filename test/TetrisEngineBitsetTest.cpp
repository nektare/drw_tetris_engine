#include <gtest/gtest.h>

#define private public
#include "../src/TetrisEngineBitset.hpp"
#undef private

// findRestingY
TEST(TetrisEngineBitset, FindRestingY_EmptyBoard_LandsAtZero) {
    TetrisEngineFast engine;
    auto piece = engine.PieceLookup['Q'];
    EXPECT_EQ(engine.findRestingY(piece, 0), 0);
}

TEST(TetrisEngineBitset, FindRestingY_BlockedRow_LandsOnTop) {
    TetrisEngineFast engine;
    engine.board.push_back({true, true, false, false, false, false, false, false, false, false});
    auto piece = engine.PieceLookup['Q']; // 2 wide
    // col 0-1 occupied at y=0, Q should rest at y=1
    EXPECT_EQ(engine.findRestingY(piece, 0), 1);
}

// placePiece
TEST(TetrisEngineBitset, PlacePiece_QAtOrigin_SetsFourCells) {
    TetrisEngineFast engine;
    auto piece = engine.PieceLookup['Q']; // (0,0),(1,0),(0,1),(1,1)
    engine.placePiece(piece, 0, 0);
    EXPECT_TRUE(engine.board[0][0]);
    EXPECT_TRUE(engine.board[0][1]);
    EXPECT_TRUE(engine.board[1][0]);
    EXPECT_TRUE(engine.board[1][1]);
}

TEST(TetrisEngineBitset, PlacePiece_GrowsBoardAsNeeded) {
    TetrisEngineFast engine;
    auto piece = engine.PieceLookup['I']; // 4 wide, 1 tall
    engine.placePiece(piece, 0, 0);
    EXPECT_EQ(engine.board.size(), 1u);
}

// clearLines
TEST(TetrisEngineBitset, ClearLines_FullRow_IsRemoved) {
    TetrisEngineFast engine;
    TetrisEngineFast::Row full;
    full.fill(true);
    engine.board.push_back(full);
    engine.clearLines();
    EXPECT_EQ(engine.board.size(), 0u);
}

TEST(TetrisEngineBitset, ClearLines_PartialRow_IsKept) {
    TetrisEngineFast engine;
    TetrisEngineFast::Row partial{};
    partial[0] = true;
    engine.board.push_back(partial);
    engine.clearLines();
    EXPECT_EQ(engine.board.size(), 1u);
}

// processLine
TEST(TetrisEngineBitset, ProcessLine_SingleQ_HeightIsTwo) {
    TetrisEngineFast engine;
    std::streambuf* orig = std::cout.rdbuf(nullptr);
    engine.processLine("Q0");
    std::cout.rdbuf(orig);
    EXPECT_EQ(engine.board.size(), 2u);
}

TEST(TetrisEngineBitset, ProcessLine_TwoQsSameColumn_HeightIsFour) {
    TetrisEngineFast engine;
    std::streambuf* orig = std::cout.rdbuf(nullptr);
    engine.processLine("Q0,Q0");
    std::cout.rdbuf(orig);
    EXPECT_EQ(engine.board.size(), 4u);
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
    TetrisEngineFast engine;
    engine.run("test/drw_test_input.txt");
    std::cout.rdbuf(orig);

    std::istringstream ss(captured.str());
    std::string line;
    int i = 0;
    while (std::getline(ss, line)) {
        // output format: "Height after [seq] = N" — extract the height value after "= "
        size_t pos = line.rfind("= ");
        std::string height = (pos != std::string::npos) ? line.substr(pos + 2) : line;
        EXPECT_EQ(height, expected[i]) << "Line " << i;
        ++i;
    }
    EXPECT_EQ(i, (int)expected.size());
}
