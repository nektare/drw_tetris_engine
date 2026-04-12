#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <vector>
#include <array>
#include <map>

class TetrisEngine {

private:
    // Very important note: 
    // Point definition uses (x, y) where x is column and y is row
    // This matches "human" way of reading a letter on X-Y coord system
    // HOWEVER, the actual engine logic uses (y,x) in calculations.
    // Hence you will see board[ty][tx] everywhere
    struct Point {
        int x, y;
    };

    std::map<char, std::vector<Point>> PieceDefinitionMap = {
        {'Q', {{0,0}, {1,0}, {0,1}, {1,1}}}, // Square
        {'Z', {{0,1}, {1,1}, {1,0}, {2,0}}}, // Horizontal Z
        {'S', {{0,0}, {1,0}, {1,1}, {2,1}}}, // Horizontal S
        {'T', {{0,1}, {1,1}, {2,1}, {1,0}}}, // T with nub pointing DOWN
        {'I', {{0,0}, {1,0}, {2,0}, {3,0}}}, // Horizontal 4-unit bar
        {'L', {{0,0}, {0,1}, {0,2}, {1,0}}}, // Vertical base stem left
        {'J', {{1,0}, {1,1}, {1,2}, {0,0}}}  // Vertical base stem right
    };

    using Row = std::array<bool, 10>;
    std::vector<Row> board;
    
    // Find the resting Y-coord for a piece starting at startX 
    int findRestingY(const std::vector<Point>& shape, int startX) {
        // Top-down scan starting from current board height down to floor
        // board height = board.size()
        for (int trialY = (int)board.size(); trialY >= 0; --trialY) {
            bool collision = false;
            for (const auto& p : shape) {
                int tx = startX + p.x;
                int ty = trialY + p.y;

                // Check floor boundary 
                if (ty < 0) {
                    collision = true;
                    break;
                }
                // Check existing blocks
                if (ty < (int)board.size() && board[ty][tx]) {
                    collision = true;
                    break;
                }
            }
            if (collision) return trialY + 1;
        }
        return 0;
    }

    // Place the piece into the board and grow the board if needed
    void placePiece(const std::vector<Point>& shape, int startX, int restingY) {
        for (const auto& p : shape) {
            int tx = startX + p.x;
            int ty = restingY + p.y;

            while (ty >= (int)board.size()) {
                board.push_back(Row{false}); // inits all 10 bools in the row to false
            }
            board[ty][tx] = true;
        }
    }

    // collect nonfull rows at one end and resize the board vector
    // to get rid of full rows in one fell swoop
    void clearLines() {
        size_t writePtr = 0;
        const size_t totalRows = board.size();
    
        for (size_t readPtr = 0; readPtr < totalRows; ++readPtr) {
            bool isFull = true;
            
            // Grid is 10 units wide
            for (size_t x = 0; x < 10; ++x) {
                if (!board[readPtr][x]) {
                    isFull = false;
                    break;
                }
            }
    
            if (!isFull) {
                // Keep this row by moving it to the write position
                if (writePtr != readPtr) {
                    board[writePtr] = std::move(board[readPtr]);
                }
                writePtr++;
            }
            // If isFull is true, the readPtr advances but writePtr stays, 
            // effectively "deleting" the full row.
        }
    
        // Shrink the vector only once at the end
        if (writePtr < totalRows) {
            board.resize(writePtr);
        }
    }

    void processLine(const std::string& line) {
        board.clear(); // Grid's initial state is empty for each line 
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, ',')) {
            if (token.empty()) continue;
            
            char type = token[0];
            int startX = std::stoi(token.substr(1)); // e.g., "Q3" -> type='Q', x=3 

            const auto& piece = PieceDefinitionMap[type];
            int restingY = findRestingY(piece, startX);
            // std::cout << "For token: [" << type << startX << "], RestingY = " << restingY << "\n"; 
            placePiece(piece, startX, restingY);
            clearLines(); // full rows removed after each piece placement
        }
        std::cout << "Height = [" << board.size() << "] after line = " << line; // line has newline at the end
    }

public:

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