#pragma once
#include <cstdint>
#include <vector>
#include "types.hpp"


class Board{
    public:
        uint64_t white_pawns, white_rooks, white_bishops, white_knights, white_queens, white_king;
        uint64_t black_pawns, black_rooks, black_bishops, black_knights, black_queens, black_king;

        bool white_to_move;
        uint8_t castling_rights;
        uint64_t en_passant_target;

        Board();
        void init_board();

        uint64_t white_pieces();
        uint64_t black_pieces();
        uint64_t all_pieces();
        uint64_t empty_squares();

        void print_binary(uint64_t);
        void print_bitboard(uint64_t);
        void print_board();

        uint64_t pseudolegal_knight_moves(uint8_t square);
        uint64_t pseudolegal_rook_moves(uint8_t square);
        uint64_t pseudolegal_bishop_moves(uint8_t square);
        uint64_t pseudolegal_queen_moves(uint8_t square);
        uint64_t pseudolegal_king_moves(uint8_t square);
        PawnMoves pseudolegal_pawn_moves();

        void toggle_piece(PieceType piece, bool is_white, uint64_t mask);
        void place_piece(int square, PieceType piece, bool is_white);
        void remove_piece(int square, PieceType piece, bool is_white);

        PieceType get_piece_at(uint8_t square, bool check_white);
        bool is_square_attacked(uint8_t square);

        void make_move(Move move);
        void unmake_move(Move move);

        std::vector<Move> generate_all_moves();

    private:
        std::vector<BoardState> history;
};