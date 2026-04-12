#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <array>
#include <algorithm>

class TetrisEngineFast {

private:
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
    std::vector<Row> board; // test spec says height can be assumed max 100
    // I was thinking of going with array instead of vector for ease of logic.
    // When I use vector, I dont have to keep track of "height". Size gives the height 
    // Hence going with vector. It is heap allocation but contiguous. 
    // So it is not that bad.

    // Find resting Y-coord using a top-down collision scan 
    int findRestingY(const Piece& piece, int startX) {

        for (int trialY = (int)board.size(); trialY >= 0; --trialY) {
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
                if (ty < (int)board.size() && board[ty][tx]) {
                    collision = true;
                    break;
                }
            }
            if (collision) return trialY + 1;
        }
        return 0;
    }

    // Place piece and grow board vertically as needed 
    void placePiece(const Piece& piece, int startX, int restingY) {
        for (const auto& p : piece.points) {
            int tx = startX + p.x;
            int ty = restingY + p.y;

            // Boundary Safety
            if (tx < 0 || tx >= 10) [[unlikely]] continue;

            while (ty >= (int)board.size()) {
                board.push_back(Row{false});
            }
            board[ty][tx] = true;
        }
    }

    // Used two pointers: O(N) cost
    // Think of writePtr as the last non full row from bottom to top
    // All rows under it non full. All above it full 
    void clearLines() {
        size_t writePtr = 0;
        const size_t totalRows = board.size();
    
        // readptr and writeptr start together
        // as below for-loop loops, readptr row is checked for full ness
        for (size_t readPtr = 0; readPtr < totalRows; ++readPtr) {

            bool isFull = true; 
            for (size_t x = 0; x < 10; ++x) {
                if (!board[readPtr][x]) {
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
                    board[writePtr] = std::move(board[readPtr]);
                }
                writePtr++;
            }
        }
    
        // since writeptr points to first full row to be discarded, resize to 
        if (writePtr < totalRows) {
            board.resize(writePtr);
        }
    }

    void processToken(std::string_view token) {
        
        char type = token[0];
        
        // test spec says piece input is 1 letter follow by 
        // leftmost col number which can only be 0-9 (since grid width  = 10)
        int startX = token[1] - '0';

        const Piece& piece = PieceLookup[type];
        
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
            int restingY = findRestingY(piece, startX);
            placePiece(piece, startX, restingY);
            clearLines();
        }
    }


    void processLine(std::string_view line) {

        board.clear(); // spec says: for each new input line containing  
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
        
        std::cout << "Height = [" << board.size() << "] after line = " << line; // line has newline at the end

    }

public:

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