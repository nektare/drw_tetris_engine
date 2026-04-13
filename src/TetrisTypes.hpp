#pragma once
#include <cstdint>
#include <array>
#include <algorithm>
#include <vector>

namespace Tetris {

    struct Point {
        int x, y;
    };

    // Fixed size Piece struct for stack allocation
    struct Piece {
        std::array<Point, 4> points;
    };

    // O(1) Lookup Table using ASCII index key
    // functions like map, but no dynamic allocation, 
    // no hashing, no pointer indirection
    // Piece definitions don't change and the memory desired is small
    // This structure is bakedinto the binary at compile time
    static constexpr std::array<Piece, 256> PieceTable = []() {
        std::array<Piece, 256> table{};
        // Coordinates {x, y} relative to the bottom-left of the piece. 
        table['Q'] = Piece{{{{0,0}, {1,0}, {0,1}, {1,1}}}}; // Square
        table['Z'] = Piece{{{{0,1}, {1,1}, {1,0}, {2,0}}}}; // Z
        table['S'] = Piece{{{{0,0}, {1,0}, {1,1}, {2,1}}}}; // S
        table['T'] = Piece{{{{0,1}, {1,1}, {2,1}, {1,0}}}}; // T nub DOWN
        table['I'] = Piece{{{{0,0}, {1,0}, {2,0}, {3,0}}}}; // Horizontal bar
        table['L'] = Piece{{{{0,0}, {0,1}, {0,2}, {1,0}}}}; // Vertical L, base stem to right
        table['J'] = Piece{{{{1,0}, {1,1}, {1,2}, {0,0}}}}; // Vertical J, base stem to left
        return table;
    }();

    struct Board {
        
        std::vector<std::array<bool, 10>> grid; // test spec says height can be assumed max 100
        // with a vector, I dont have to keep track of "height". Size gives the height 
        // Its heap allocation is contiguous. So it is not that bad.
        
        inline int getHeight() {
            return grid.size();
        }

        inline void reset() {
            grid.clear(); // spec says: for each new input line start with empty board
        }

        // Helper for collision detection to keep findRestingY clean
        inline bool collides(const Piece& piece, int startX, int trialY) const {
            for (const auto& p : piece.points) {
                int tx = startX + p.x;
                int ty = trialY + p.y;
                if (ty < 0) return true;
                if (ty < (int)grid.size() && grid[ty][tx]) return true;
            }
            return false;
        }

        // Find resting Y-coord using a top-down collision scan 
        inline int findRestingY(const Piece& piece, int startX) const {
            for (int trialY = (int)grid.size(); trialY >= 0; --trialY) {
                if (collides(piece, startX, trialY)) return trialY + 1;
            }
            return 0;
        }

        // Place piece and grow board vertically as needed 
        inline void place(const Piece& piece, int startX, int restingY) {
            for (const auto& p : piece.points) {
                int tx = startX + p.x;
                int ty = restingY + p.y;

                // Boundary Safety
                if (tx < 0 || tx >= 10) [[unlikely]] continue;

                while (ty >= (int)grid.size()) {
                    grid.push_back(std::array<bool, 10>{false});
                }
                grid[ty][tx] = true;
            }
        }

        // Used two pointers: O(N) cost
        // Think of writePtr as the last non full row from bottom to top
        // All rows under it non full. All above it full 
        inline void clearFullLines() {
            size_t writePtr = 0;
            const size_t totalRows = grid.size();
        
            // readptr and writeptr start together
            // as below for-loop loops, readptr row is checked for full ness
            for (size_t readPtr = 0; readPtr < totalRows; ++readPtr) {
                bool isFull = true; 
                for (size_t x = 0; x < 10; ++x) {
                    if (!grid[readPtr][x]) {
                        isFull = false;
                        break;
                    }
                }
                // if the row of readptr is not full, writeptr and readptr advance 
                // Imp Note: at full rows, writeptr would point to full row at PREVIOUS
                // iteration. Thus writeptr starts point to first full row. 
                // As readptr moves to next row
                // if it also full, writeptr still stays at the previous full row
                // this continues till readptr hits a non-full row at this point, wrtitepr ! readptr and 
                // that nonfull row gets moved to botton first row that needs discarding.
                // EFFECTIVELY : Writeptr always points to the FIRST FULL ROW 
                // with all non-full rows below it 
                if (!isFull) {
                    if (writePtr != readPtr) {
                        grid[writePtr] = std::move(grid[readPtr]);
                    }
                    writePtr++;
                }
            }
        
            // since writeptr points to first full row to be discarded, resize to writeptr
            if (writePtr < totalRows) {
                grid.resize(writePtr);
            }
        }
    };
}