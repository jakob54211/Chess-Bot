#include <cstdint>
#include <iostream>
#include <tuple>
#include <vector>

class Board {
public:
    void set_start_position();
    void print_board();

private:
    std::tuple<bool, int> piece_at(uint8_t square);
    std::vector<uint8_t> knight_moves(uint8_t square);
    std::vector<uint8_t> bishop_moves(uint8_t square);
    std::vector<uint8_t> rook_moves(uint8_t square);
    std::vector<uint8_t> pawn_moves(uint8_t square);
    std::vector<uint8_t> queen_moves(uint8_t square);
    std::vector<uint8_t> king_moves(uint8_t square);
    std::vector<uint8_t> all_valid_moves(bool colour);
    bool is_in_check(bool colour);

struct Pieces {
    uint64_t pawns   = 0;
    uint64_t knights = 0;
    uint64_t bishops = 0;
    uint64_t rooks   = 0;
    uint64_t queens  = 0;
    uint64_t king    = 0;
};

    Pieces white;
    Pieces black;

};

void Board::set_start_position() {
    // Start for white
    white.pawns   = 0x000000000000FF00ULL;
    white.knights = 0x0000000000000042ULL;
    white.bishops = 0x0000000000000024ULL;
    white.rooks   = 0x0000000000000081ULL;
    white.queens  = 0x0000000000000010ULL;
    white.king    = 0x0000000000000008ULL;

    // Start for black
    black.pawns   = 0x00FF000000000000ULL;
    black.knights = 0x4200000000000000ULL;
    black.bishops = 0x2400000000000000ULL;
    black.rooks   = 0x8100000000000000ULL;
    black.queens  = 0x1000000000000000ULL;
    black.king    = 0x0800000000000000ULL;
}

/* 
    Returns a tuple representing which piece is on the given square
    nothing = 0;
    Pawn    = 1
    knight  = 2
    bishop  = 3
    rook    = 4
    queen   = 5
    king    = 6

    white = True
    black = False
*/
std::tuple<bool, int> Board::piece_at(uint8_t square) {
    uint64_t bit = 1ULL << square;

    if (bit & white.pawns)   return {true, 1};
    if (bit & white.knights) return {true, 2};
    if (bit & white.bishops) return {true, 3};
    if (bit & white.rooks)   return {true, 4};
    if (bit & white.queens)  return {true, 5};
    if (bit & white.king)    return {true, 6};

    if (bit & black.pawns)   return {false, 1};
    if (bit & black.knights) return {false, 2};
    if (bit & black.bishops) return {false, 3};
    if (bit & black.rooks)   return {false, 4};
    if (bit & black.queens)  return {false, 5};
    if (bit & black.king)    return {false, 6};

    return {false, 0}; // Empty square
}

void Board::print_board() {
    const char* white_symbols = ".PNBRQK";
    const char* black_symbols = ".pnbrqk";

    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << "  ";

        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + (7 - file);
            auto [is_white, piece] = piece_at(square);

            char symbol = is_white
                ? white_symbols[piece]
                : black_symbols[piece];

            std::cout << symbol << ' ';
        }

        std::cout << '\n';
    }

    std::cout << "\n   a b c d e f g h\n";
}

std::vector<uint8_t> Board::knight_moves(uint8_t square) {
    std::vector<uint8_t> valid_moves;

    auto [colour, piece] = piece_at(square);
    bool player_colour   = colour;

    int row = square / 8;
    int col = square % 8; 
    
    const int offsets[8][2] = {
        { 2, 1}, { 2, -1},
        {-2, 1}, {-2, -1},
        { 1, 2}, { 1, -2},
        {-1, 2}, {-1, -2}
    };
    
    for (const auto& [dr, dc] : offsets) {
        int new_row = row + dr;
        int new_col = col + dc;

        if (new_row < 0 || new_row >= 8 ||
            new_col < 0 || new_col >= 8) {
            continue;
        }

        uint8_t target = static_cast<uint8_t>(new_row * 8 + new_col);
        auto [target_colour, target_piece] = piece_at(target);

        if (target_colour != player_colour || target_piece == 0) {
            valid_moves.push_back(target);
        }
    }
    return valid_moves;
}

std::vector<uint8_t> Board::bishop_moves(uint8_t square) {
    std::vector<uint8_t> valid_moves;
    auto [colour, piece] = piece_at(square);
    bool player_colour = colour;

    int row = square / 8;
    int col = square % 8;

    const int directions[4][2] = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    for (const auto& [dr, dc] : directions) {
        int new_row = row + dr;   
        int new_col = col + dc;

        while (new_row >= 0 && new_row < 8 &&
               new_col >= 0 && new_col < 8) {

            uint8_t target = static_cast<uint8_t>(new_row * 8 + new_col);
            auto [target_colour, target_piece] = piece_at(target);
            
            if (target_piece == 0) {
                valid_moves.push_back(target);
            } else if (target_colour != player_colour) {
                valid_moves.push_back(target);
                break;
            } else {
                break;
            } 

            new_row += dr;
            new_col += dc;
        }
    }
    return valid_moves;
}

std::vector<uint8_t> Board::rook_moves(uint8_t square) {
    std::vector<uint8_t> valid_moves;

    auto [colour, piece] = piece_at(square);
    bool player_colour   = colour;

    int row = square / 8;
    int col = square % 8;

    const int directions[4][2] {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    for (const auto& [dr, dc] : directions) {

        int new_row = row + dr;
        int new_col = col + dc;

        while (new_row >= 0 && new_row < 8 &&
               new_col >= 0 && new_col < 8) {

            uint8_t target = static_cast<uint8_t>(new_row * 8 + new_col);
            auto [target_colour, target_piece] = piece_at(target);
            
            if (target_piece == 0) {
                valid_moves.push_back(target);
            } else if (target_colour != player_colour) {
                valid_moves.push_back(target);
                break;
            } else {
                break;
            } 

            new_row += dr;
            new_col += dc;
        }

    }
    return valid_moves;
}

std::vector<uint8_t> Board::pawn_moves(uint8_t square) {
    std::vector<uint8_t> valid_moves;

    if (square >= 64) {
        return valid_moves;
    }

    auto [player_colour, piece] = piece_at(square);
    if (piece != 1) {
        return valid_moves;
    }

    int row = square / 8;
    int col = square % 8;
    int direction = player_colour ? 1 : -1;
    int start_row = player_colour ? 1 : 6;
    int max_steps = (row == start_row) ? 2 : 1;

    for (int step = 1; step <= max_steps; ++step) {
        int new_row = row + step * direction;
        if (new_row < 0 || new_row >= 8) {
            break;
        }

        uint8_t target = static_cast<uint8_t>(new_row * 8 + col);
        if (std::get<1>(piece_at(target)) != 0) {
            break;
        }

        valid_moves.push_back(target);
    }

    int new_row = row + direction;
    if (new_row < 0 || new_row >= 8) {
        return valid_moves;
    }

    for (int dc : {-1, 1}) {
        int new_col = col + dc;
        if (new_col < 0 || new_col >= 8) {
            continue;
        }

        uint8_t target = static_cast<uint8_t>(new_row * 8 + new_col);
        auto [target_colour, target_piece] = piece_at(target);
        if (target_piece != 0 && target_colour != player_colour) {
            valid_moves.push_back(target);
        }
    }

    return valid_moves;
}

std::vector<uint8_t> Board::queen_moves(uint8_t square) {
    std::vector<uint8_t> valid_moves;

    auto [colour, piece] = piece_at(square);
    bool player_colour   = colour;

    int row = square / 8;
    int col = square % 8;

    const int directions[8][2] = {
        { 1, 0}, { -1, 0},
        { 0, 1}, { 0, -1},
        { 1, 1}, { 1, -1},
        {-1, 1}, {-1, -1}
    };

    for (const auto& [dr, dc] : directions) {

        int new_row = row + dr;
        int new_col = col + dc;

        while (new_row >= 0 && new_row < 8 &&
               new_col >= 0 && new_col < 8) {

            uint8_t target = static_cast<uint8_t>(new_row * 8 + new_col);
            auto [target_colour, target_piece] = piece_at(target);
            
            if (target_piece == 0) {
                valid_moves.push_back(target);
            } else if (target_colour != player_colour) {
                valid_moves.push_back(target);
                break;
            } else {
                break;
            } 

            new_row += dr;
            new_col += dc;
        }

    }
    return valid_moves;
}

std::vector<uint8_t> Board::all_valid_moves(bool colour) {
    uint64_t all_pieces;
    if (colour) {
        all_pieces = black.pawns | black.knights | black.bishops |
                    black.rooks | black.queens | black.king;
    } else {
        all_pieces = white.pawns | white.knights | white.bishops |
                    white.rooks | white.queens | white.king;
    }

    std::vector<uint8_t> all_valid_moves;
    std::vector<uint8_t> temp;

    for (size_t i = 0; i < 64; i++) {
        if (1ULL << i & (all_pieces)) {
            auto [colour, piece] = piece_at(i);

            switch (piece) {
                case 2: temp = knight_moves(i); break;
                case 3: temp = bishop_moves(i); break;
                case 4: temp = rook_moves(i); break;
                case 5: temp = queen_moves(i); break;
                default: continue;
            }

            for (uint8_t target : temp) {
                all_valid_moves.push_back(target);
            }
        }
    }
    return all_valid_moves;
}

bool Board::is_in_check(bool colour) {

    auto all_moves = all_valid_moves(colour);
    uint64_t king = colour ? white.king : black.king;

    for (uint8_t target : all_moves) {
        if (king & (1ULL << target)) {
            return true;
        }
    }
    return false;
}

int main() {
    Board board;
    board.set_start_position();
    board.print_board();

    return 0;
}
