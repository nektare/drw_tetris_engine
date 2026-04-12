#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include "TetrisTypes.hpp"

class TetrisEngineBitset {
private:
    Tetris::BitBoard board;

    // Branchless O(1) lookup
    inline constexpr const Tetris::BitPiece& getPiece(char type) const {
        return Tetris::PieceTable[static_cast<unsigned char>(type)];
    }

    void processToken(std::string_view token) {
        if (token.length() < 2) return;

        char type = token[0];
        int x = token[1] - '0';
        const auto& p = getPiece(type);

        // Validation: Ignore invalid tokens or out-of-bounds X
        if (p.h == 0 || x < 0 || x > 9) [[unlikely]] return;

        // Find Resting Y via top-down scan
        int restingY = 0;
        for (int y = board.height; y >= 0; --y) {
            if (board.collides(p, x, y)) {
                restingY = y + 1;
                break;
            }
        }

        board.place(p, x, restingY);
        board.clearFullLines();
    }

public:
    TetrisEngineBitset() = default;

    void processLine(std::string_view line) {
        board.reset();

        size_t start = 0;
        while (start < line.size()) {
            size_t end = line.find(',', start);
            std::string_view token = line.substr(start, (end == std::string_view::npos) 
                                     ? std::string_view::npos : end - start);

            if (!token.empty()) [[likely]] {
                processToken(token);
            }

            if (end == std::string_view::npos) break;
            start = end + 1;
        }
        std::cout << "Height = [" << board.height << "] after line = " << line;
    }

    void run(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return;
    
        std::string line;
        while (std::getline(file, line)) { 
            if (!line.empty()) processLine(line);
        }
    }
};