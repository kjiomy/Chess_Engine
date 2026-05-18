#pragma once
#include <cstdint>
#include <vector>
#include "types.hpp"


class Board{
    public:
        uint64_t bitboards[2][7];
        PieceType piece_list[64];

        bool white_to_move;
        uint8_t castling_rights;
        uint64_t en_passant_target;

        Board();
        void init_board();

        inline uint64_t white_pieces() const{
            return bitboards[WHITE][PAWN] | bitboards[WHITE][KNIGHT] | bitboards[WHITE][BISHOP] | bitboards[WHITE][ROOK] | bitboards[WHITE][QUEEN] | bitboards[WHITE][KING];
        }

        inline uint64_t black_pieces() const{
            return bitboards[BLACK][PAWN] | bitboards[BLACK][KNIGHT] | bitboards[BLACK][BISHOP] | bitboards[BLACK][ROOK] | bitboards[BLACK][QUEEN] | bitboards[BLACK][KING];
        }

        inline uint64_t all_pieces() const{
            return white_pieces() | black_pieces();
        }

        inline uint64_t empty_squares() const {
            return ~all_pieces();
        }

        void print_board();



        inline void place_piece(int square, PieceType piece, bool is_white){
            if(piece == EMPTY) return;
            uint64_t place_mask = 1ULL << square;

            bitboards[is_white][piece] |= place_mask;
            piece_list[square] = piece;
        }

        inline void remove_piece(int square, PieceType piece, bool is_white){
            if(piece == EMPTY) return;
            uint64_t remove_mask = ~(1ULL << square);

            bitboards[is_white][piece] &= remove_mask;
            piece_list[square] = EMPTY;
        }

        inline void move_piece(int from, int to, bool is_white, PieceType piece){
            if(piece == EMPTY) return;
            uint64_t move_mask = (1ULL << from) | (1ULL << to);

            bitboards[is_white][piece] ^= move_mask;
            piece_list[from] = EMPTY;
            piece_list[to] = piece;
        }

        inline PieceType get_piece_at(uint8_t square) const{
            return piece_list[square];
        }
        bool is_square_attacked(uint8_t square, bool is_white);

        BoardState make_move(Move move);
        void unmake_move(Move move, BoardState prev_state);
};