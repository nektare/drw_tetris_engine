#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include "TetrisTypes.hpp"

class TetrisEngineFast {
private:
    Tetris::FastBoard board;

    void processToken(std::string_view token) {
        char type = token[0];
        
        // test spec says piece input is 1 letter follow by 
        // leftmost col number which can only be 0-9 (since grid width  = 10)
        int startX = token[1] - '0';

        const Tetris::Piece& piece = Tetris::PieceLookup[static_cast<unsigned char>(type)];
        
        // Boundary validation (skip piece if boundary violated)
        // test exercise pdf says I may ignore, this is just an 
        // extra check to ack the possibility and highlight
        // compiler guidance hints.

        // One note: I have used these hints only for extremities
        // so that all hot-path logic stays together and improbably 
        // code gets pushed farther away from hot path.

        bool outOfBounds = false;
        for (const auto& p : piece.points) {
            if (startX + p.x >= 10) [[unlikely]] {
                outOfBounds = true;
                break;
            }
        }
        if (!outOfBounds) [[likely]] {
            int restingY = board.findRestingY(piece, startX);
            board.place(piece, startX, restingY);
            board.clearFullLines();
        }
    }

public:
    TetrisEngineFast() = default;

    void processLine(std::string_view line) {
        board.reset(); // spec says: for each new input line containing  
                       // pieces sequence, start with empty board
        
        size_t start = 0;
        while (start < line.size()) {

            size_t end = line.find(',', start);
            
            // Slice the token without copying - highlighting why I used string_view 
            // Had I pass const std:string&, substr would have resulted in new heap allocation
            std::string_view token = line.substr(start, (end == std::string_view::npos) ? 
                                                            std::string_view::npos : end - start);

            // better branch prediction with [[likely]]
            // This is compiled with C++20, prior to that intrinsic was __builtin_expect
            // Compiler assumes non-empty token to be the hot path and 
            // keeps CPU instruction pipeline full 
            
            if (!token.empty()) {
                processToken(token);
            }

            // if we just processed the last piece, break out of the line processing loop
            if (end == std::string_view::npos) break;

            start = end + 1;
        }
        
        std::cout << "Height = [" << board.grid.size() << "] after line = " << line; 
    }

    void run(const std::string& filename) {
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