#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include <cstdint>

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
    static constexpr std::array<Piece, 256> PieceLookup = []() {
        std::array<Piece, 256> lookup{};
        // Coordinates {x, y} relative to the bottom-left of the piece. 
        lookup['Q'] = Piece{{{{0,0}, {1,0}, {0,1}, {1,1}}}}; // Square
        lookup['Z'] = Piece{{{{0,1}, {1,1}, {1,0}, {2,0}}}}; // Z
        lookup['S'] = Piece{{{{0,0}, {1,0}, {1,1}, {2,1}}}}; // S
        lookup['T'] = Piece{{{{0,1}, {1,1}, {2,1}, {1,0}}}}; // T nub DOWN
        lookup['I'] = Piece{{{{0,0}, {1,0}, {2,0}, {3,0}}}}; // Horizontal bar
        lookup['L'] = Piece{{{{0,0}, {0,1}, {0,2}, {1,0}}}}; // Vertical L, base stem to right
        lookup['J'] = Piece{{{{1,0}, {1,1}, {1,2}, {0,0}}}}; // Vertical J, base stem to left
        return lookup;
    }();

    using Row = std::array<bool, 10>;

    struct FastBoard {
        std::vector<Row> grid; // test spec says height can be assumed max 100
        // I was thinking of going with array instead of vector for ease of logic.
        // When I use vector, I dont have to keep track of "height". Size gives the height 
        // Hence going with vector. It is heap allocation but contiguous. 
        // So it is not that bad.

        inline void reset() {
            grid.clear(); // spec says: for each new input line start with empty board
        }

        // Find resting Y-coord using a top-down collision scan 
        inline int findRestingY(const Piece& piece, int startX) const {
            for (int trialY = (int)grid.size(); trialY >= 0; --trialY) {
                bool collision = false;
                for (const auto& p : piece.points) {
                    int tx = startX + p.x;
                    int ty = trialY + p.y;

                    // Boundary Safety: Ensure we don't access column 10+ 
                    if (tx < 0 || tx >= 10) [[unlikely]] continue; 

                    if (ty < 0) {
                        collision = true;
                        break;
                    }
                    if (ty < (int)grid.size() && grid[ty][tx]) {
                        collision = true;
                        break;
                    }
                }
                if (collision) return trialY + 1;
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
                    grid.push_back(Row{false});
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
        
            // since writeptr points to first full row to be discarded, resize to 
            if (writePtr < totalRows) {
                grid.resize(writePtr);
            }
        }
    };
}