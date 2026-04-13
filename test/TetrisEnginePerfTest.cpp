#include <iostream>
#include <iomanip>
#include <chrono>
#include <string_view>

#include "../src/TetrisEngine.hpp"
#include "../src/TetrisTypes.hpp"
#include "../src/TetrisTypesBitset.hpp"

using StandardEngine = TetrisEngine<Tetris::Board,    Tetris::Piece,    Tetris::PieceTable>;
using BitsetEngine   = TetrisEngine<Tetris::BitBoard, Tetris::BitPiece, Tetris::BitPieceTable>;

static constexpr std::string_view lines[] = {
    "I0,I4,Q8",
    "T1,Z3,I4",
    "Q0,I2,I6,I0,I6,I6,Q2,Q4",
    "Q0,Q2,Q4,Q6,Q8",
    "Q0,Q1",
    "L0,J2,L4,J6,Q8",
    "I0,I0,I0,I0",
    "T0,T3,T6,T8",
    "Z0,S2,Z4,S6,I6",
    "L0,L1,L2,L3,L4,L5,L6,L7,L8",
};

static constexpr int ITERATIONS = 1'000'000;

template<typename Engine>
long long runBench(const char* label) {
    Engine engine;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; ++i) {
        for (const auto& line : lines) {
            engine.processLine(line);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << label << ": " << ms << " ms  (" << ITERATIONS * std::size(lines) << " lines)\n";
    return ms;
}

int main() {
    // suppress per-line height output
    std::cout.setstate(std::ios::failbit);
    long long standard = runBench<StandardEngine>("StandardEngine");
    long long bitset   = runBench<BitsetEngine>  ("BitsetEngine  ");
    std::cout.clear();

    std::cout << "\nStandardEngine : " << standard << " ms\n";
    std::cout << "BitsetEngine   : " << bitset   << " ms\n";
    std::cout << "Speedup        : " << std::fixed << std::setprecision(2)
              << (static_cast<double>(standard) / std::max(bitset, 1LL)) << "x\n";
}
