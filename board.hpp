#pragma once
#include <cstdint>

// Defining some filter to calculate legal moves
constexpr uint64_t NOT_A_FILE  = 0xFEFEFEFEFEFEFEFE;
constexpr uint64_t NOT_H_FILE = 0x7F7F7F7F7F7F7F7F;
constexpr uint64_t NOT_AB_FILE = 0xFCFCFCFCFCFCFCFC;
constexpr uint64_t NOT_GH_FILE = 0x3F3F3F3F3F3F3F3F;
constexpr uint64_t MASK_64 = 0xFFFFFFFFFFFFFFFF;

struct PawnMoves{
    uint64_t single_push;
    uint64_t double_push;
    uint64_t capture_left;
    uint64_t capture_right;
    uint64_t get_all(){
        return single_push | double_push | capture_left | capture_right;
    }
};


class Board{
    public:
        uint64_t white_pawns, white_rooks, white_bishops, white_knights, white_queens, white_king;
        uint64_t black_pawns, black_rooks, black_bishops, black_knights, black_queens, black_king;

        bool white_to_move;
        //attualmente non gestito
        uint64_t en_passant_target;

        Board();
        void init_board();

        uint64_t white_pieces();
        uint64_t black_pieces();
        uint64_t all_pieces();
        uint64_t empty_squares();

        void print_binary(uint64_t);
        void print_bitboard(uint64_t);

        uint64_t knight_attacks(uint64_t square, uint64_t friendly_pieces);
        PawnMoves white_pawn_moves();
        PawnMoves black_pawn_moves();
};