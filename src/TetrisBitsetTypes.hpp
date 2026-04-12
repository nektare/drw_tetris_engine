#pragma once
#include <cstdint>
#include <array>
#include <algorithm>

namespace Tetris {
    struct BitPiece {
        uint16_t rows[4]; // Row-wise bitmasks
        int h;            // Vertical span
    };

    // FIXED: Using std::array allows the lambda to return the table by value.
    // In C++, raw arrays decay to pointers, which cannot initialize constexpr.
    // std::array is a LiteralType and maintains identical memory layout to T[N].
    static constexpr std::array<BitPiece, 256> PieceTable = []() {
        std::array<BitPiece, 256> table{}; 
        // Bits represent column offsets; rows[0] is the bottom row of the piece
        table['Q'] = {{0b11, 0b11, 0, 0}, 2};   
        table['Z'] = {{0b110, 0b011, 0, 0}, 2}; 
        table['S'] = {{0b011, 0b110, 0, 0}, 2}; 
        table['T'] = {{0b010, 0b111, 0, 0}, 2}; 
        table['I'] = {{0b1111, 0, 0, 0}, 1};    
        table['L'] = {{0b11, 0b01, 0b01, 0}, 3};
        table['J'] = {{0b11, 0b10, 0b10, 0}, 3};
        return table;
    }();

    struct BitBoard {
        uint16_t rows[128]; // Fixed-size array for L1 cache residency
        int height;

        BitBoard() : rows{}, height(0) {}

        inline void reset() {
            std::fill(std::begin(rows), std::end(rows), 0);
            height = 0;
        }

        // O(1) collision check per row using bitwise AND
        inline bool collides(const BitPiece& p, int x, int y) const {
            for (int i = 0; i < p.h; ++i) {
                if ((rows[y + i] & (p.rows[i] << x)) != 0) return true;
            }
            return false;
        }

        // O(1) bitwise placement using OR
        inline void place(const BitPiece& p, int x, int y) {
            for (int i = 0; i < p.h; ++i) {
                rows[y + i] |= (p.rows[i] << x);
            }
            height = std::max(height, y + p.h);
        }

        // Two-pointer line clearing: O(N) with zero heap reallocations
        inline void clearFullLines() {
            int writePtr = 0;
            for (int readPtr = 0; readPtr < height; ++readPtr) {
                // 0x3FF represents a full row (10 bits set)
                if (rows[readPtr] != 0x3FF) {
                    if (writePtr != readPtr) rows[writePtr] = rows[readPtr];
                    writePtr++;
                }
            }
            // Zero out stale rows to maintain deterministic state
            for (int i = writePtr; i < height; ++i) rows[i] = 0;
            height = writePtr;
        }
    };
}