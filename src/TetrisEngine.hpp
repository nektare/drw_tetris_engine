#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

// Templatized to accept either Standard or Bitset types
// PieceTable is passed as a constant reference to the static constexpr std::array
template <typename BoardT, typename PieceT, const auto& PieceTable>
class TetrisEngine {
private:
    BoardT board;

    // Branchless O(1) lookup: no pointer indirection or hashing
    inline constexpr const PieceT& getPiece(char type) const {
        return PieceTable[static_cast<unsigned char>(type)];
    }

    void processToken(std::string_view token) {
        
        if (token.length() < 2) [[unlikely]] return;

        char type = token[0];
        
        // test spec says piece input is 1 letter follow by 
        // leftmost col number which can only be 0-9 (since grid width = 10)
        int startX = token[1] - '0';
        const auto& piece = getPiece(type);

        int restingY = board.findRestingY(piece, startX);
        board.place(piece, startX, restingY);
        board.clearFullLines();
    }

public:
    TetrisEngine() = default;

    void processLine(std::string_view line) {
        board.reset(); // spec says: for each new input line start with empty board
        
        size_t start = 0;
        while (start < line.size()) {
            size_t end = line.find(',', start);
            
            // Slice the token without copying - highlighting why I used string_view 
            // Had I pass const std:string&, substr would have resulted in new heap allocation
            std::string_view token = line.substr(start, (end == std::string_view::npos) ? 
                                                            std::string_view::npos : end - start);

            if (!token.empty()) {
                processToken(token);
            }

            // if we just processed the last piece, break out of the line processing loop
            if (end == std::string_view::npos) {
                break;
            }

            start = end + 1;
        }
        
        std::cout << "Height = [" << board.getHeight() << "] after line = " << line; 
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