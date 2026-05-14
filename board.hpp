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

        inline uint64_t white_pieces() const{
            return white_pawns | white_bishops | white_king | white_knights | white_rooks | white_queens;
        }

        inline uint64_t black_pieces() const{
            return black_pawns | black_rooks | black_bishops | black_knights | black_queens | black_king;
        }

        inline uint64_t all_pieces() const{
            return white_pieces() | black_pieces();
        }

        inline uint64_t empty_squares() const {
            return ~all_pieces();
        }

        void print_board();

        void toggle_piece(PieceType piece, bool is_white, uint64_t mask);
        void place_piece(int square, PieceType piece, bool is_white);
        void remove_piece(int square, PieceType piece, bool is_white);

        PieceType get_piece_at(uint8_t square, bool check_white);
        bool is_square_attacked(uint8_t square, bool is_white);

        BoardState make_move(Move move);
        void unmake_move(Move move, BoardState prev_state);

        void generate_all_moves(MoveList &list);

};