#pragma once
#include <cstdint>
#include <array>
#include <algorithm>

namespace Tetris {
    struct BitPiece {
        uint16_t rows[4];
        int h;
    };

    // Constant data baked into the .rodata segment
    static constexpr BitPiece PieceTable[256] = []() {
        BitPiece table[256] = {};
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
        uint16_t rows[128]; 
        int height;

        BitBoard() : rows{}, height(0) {}

        inline void reset() {
            std::fill(std::begin(rows), std::end(rows), 0);
            height = 0;
        }

        inline bool collides(const BitPiece& p, int x, int y) const {
            for (int i = 0; i < p.h; ++i) {
                if ((rows[y + i] & (p.rows[i] << x)) != 0) return true;
            }
            return false;
        }

        inline void place(const BitPiece& p, int x, int y) {
            for (int i = 0; i < p.h; ++i) {
                rows[y + i] |= (p.rows[i] << x);
            }
            height = std::max(height, y + p.h);
        }

        inline void clearFullLines() {
            int writePtr = 0;
            for (int readPtr = 0; readPtr < height; ++readPtr) {
                if (rows[readPtr] != 0x3FF) {
                    if (writePtr != readPtr) rows[writePtr] = rows[readPtr];
                    writePtr++;
                }
            }
            for (int i = writePtr; i < height; ++i) rows[i] = 0;
            height = writePtr;
        }
    };
}