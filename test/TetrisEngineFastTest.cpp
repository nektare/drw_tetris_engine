#include <gtest/gtest.h>

#define private public
#include "../src/TetrisEngineFast.hpp"
#undef private

// findRestingY
TEST(TetrisEngineFastTest, FindRestingY_EmptyBoard_LandsAtZero) {
    TetrisEngineFast engine;
    auto piece = Tetris::PieceLookup['Q'];
    EXPECT_EQ(engine.board.findRestingY(piece, 0), 0);
}

TEST(TetrisEngineFastTest, FindRestingY_BlockedRow_LandsOnTop) {
    TetrisEngineFast engine;
    engine.board.grid.push_back({true, true, false, false, false, false, false, false, false, false});
    auto piece = Tetris::PieceLookup['Q']; // 2 wide
    EXPECT_EQ(engine.board.findRestingY(piece, 0), 1);
}

// place
TEST(TetrisEngineFastTest, PlacePiece_QAtOrigin_SetsFourCells) {
    TetrisEngineFast engine;
    auto piece = Tetris::PieceLookup['Q']; // (0,0),(1,0),(0,1),(1,1)
    engine.board.place(piece, 0, 0);
    EXPECT_TRUE(engine.board.grid[0][0]);
    EXPECT_TRUE(engine.board.grid[0][1]);
    EXPECT_TRUE(engine.board.grid[1][0]);
    EXPECT_TRUE(engine.board.grid[1][1]);
}

TEST(TetrisEngineFastTest, PlacePiece_GrowsBoardAsNeeded) {
    TetrisEngineFast engine;
    auto piece = Tetris::PieceLookup['I']; // 4 wide, 1 tall
    engine.board.place(piece, 0, 0);
    EXPECT_EQ(engine.board.grid.size(), 1u);
}

// clearFullLines
TEST(TetrisEngineFastTest, ClearLines_FullRow_IsRemoved) {
    TetrisEngineFast engine;
    Tetris::Row full;
    full.fill(true);
    engine.board.grid.push_back(full);
    engine.board.clearFullLines();
    EXPECT_EQ(engine.board.grid.size(), 0u);
}

TEST(TetrisEngineFastTest, ClearLines_PartialRow_IsKept) {
    TetrisEngineFast engine;
    Tetris::Row partial{};
    partial[0] = true;
    engine.board.grid.push_back(partial);
    engine.board.clearFullLines();
    EXPECT_EQ(engine.board.grid.size(), 1u);
}

// processLine
TEST(TetrisEngineFastTest, ProcessLine_SingleQ_HeightIsTwo) {
    TetrisEngineFast engine;
    engine.processLine("Q0");
    EXPECT_EQ(engine.board.grid.size(), 2u);
}

TEST(TetrisEngineFastTest, ProcessLine_TwoQsSameColumn_HeightIsFour) {
    TetrisEngineFast engine;
    engine.processLine("Q0,Q0");
    EXPECT_EQ(engine.board.grid.size(), 4u);
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
    engine.run("test/data/drw_test_input.txt");
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
