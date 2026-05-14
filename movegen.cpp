#include "movegen.hpp"

namespace MoveGen{
    uint64_t pseudolegal_knight_moves(uint8_t square, Board &board){
        uint64_t k = 1ULL << square;
        uint64_t friendly_pieces = board.white_to_move ? board.white_pieces() : board.black_pieces();

        uint64_t attacks = (k << 17) & NOT_A_FILE;
        attacks |= (k << 15) & NOT_H_FILE;
        attacks |= (k << 10) & NOT_AB_FILE;
        attacks |= (k <<  6) & NOT_GH_FILE;
        attacks |= (k >> 17) & NOT_H_FILE;
        attacks |= (k >> 15) & NOT_A_FILE;
        attacks |= (k >> 10) & NOT_GH_FILE;
        attacks |= (k >>  6) & NOT_AB_FILE;
        attacks = attacks & ~friendly_pieces;

        return attacks;
    }
}