#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <array>
#include <algorithm>
#include <cstdint>

class TetrisEngineFast {

private:
    // Each row is viewed as a 10-bit integer. 0x3FF represents a full row (1111111111).
    // Using a fixed array of 128 to stay in L1 cache and avoid heap allocation.
    uint16_t board[128]; 
    int currentHeight;

    // LUT for shapes. Each shape is a 4-integer array (row masks).
    // Bottom row of the piece is index 0.
    struct Piece { uint16_t rows[4]; int h; };
    
    Piece getPiece(char type) {
        switch (type) {
            case 'Q': return {{0b11, 0b11, 0, 0}, 2};   
            case 'Z': return {{0b110, 0b011, 0, 0}, 2}; 
            case 'S': return {{0b011, 0b110, 0, 0}, 2}; 
            case 'T': return {{0b010, 0b111, 0, 0}, 2}; 
            case 'I': return {{0b1111, 0, 0, 0}, 1};    
            case 'L': return {{0b11, 0b01, 0b01, 0}, 3};
            case 'J': return {{0b11, 0b10, 0b10, 0}, 3};
            default: return {{0, 0, 0, 0}, 0};
        }
    }

    // find the bottom y to rest on for the piece
    inline int findRestingY(const Piece& p, int startX) {
        uint16_t pMasks[4];
        for (int i = 0; i < 4; ++i) pMasks[i] = p.rows[i] << startX;

        // Top-down scan for the first collision 
        for (int trialY = currentHeight; trialY >= 0; --trialY) {
            bool collision = false;
            for (int i = 0; i < p.h; ++i) {
                // Bitwise AND for O(1) collision check per row
                if ((board[trialY + i] & pMasks[i]) != 0) {
                    collision = true;
                    break;
                }
            }
            if (collision) return trialY + 1;
        }
        return 0;
    }

    // Mark the cells (bits) to 1 when a piece is placed.
    // Update the current height
    inline void placePiece(const Piece& p, int startX, int restingY) {
        for (int i = 0; i < p.h; ++i) {
            // bitwise OR for O(1) placement
            board[restingY + i] |= (p.rows[i] << startX);
        }
        currentHeight = std::max(currentHeight, restingY + p.h);
    }

    // detect full lines after each piece placement (10 bits show 0x3FF)
    // 
    inline void clearLines() {
        for (int y = currentHeight - 1; y >= 0; --y) {
            if (board[y] == 0x3FF) { // 0x3FF = 10 bits set 
                // Shift all rows above down by one
                for (int i = y; i < currentHeight; ++i) {
                    board[i] = board[i + 1];
                }
                board[currentHeight] = 0;
                currentHeight--;
            }
        }
    }

    void processLine(const std::string& line) {
        // Reset board for every line 
        std::fill(std::begin(board), std::end(board), 0);
        currentHeight = 0;

        size_t pos = 0;
        while (pos < line.length()) {
            char type = line[pos];
            int startX = line[pos + 1] - '0';

            Piece p = getPiece(type);
            int restingY = findRestingY(p, startX);
            placePiece(p, startX, restingY);
            clearLines();

            pos += (pos + 2 < line.length() && line[pos + 2] == ',') ? 3 : 2;
        }
        std::cout << currentHeight << std::endl;
    }

public:

    // Constructor : clean initial state
    TetrisEngine() : board{}, currentHeight(0) {}

    void run(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return;
        }
    
        std::string line;
        while (std::getline(file, line)) { 
            if (!line.empty()) {
                processLine(line);
            }
        }
        file.close();
    }
};