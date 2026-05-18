#pragma once
#include <cstdint>
#include "types.hpp"
#include "board.hpp"



namespace MoveGen{
    inline uint64_t knight_masks[64];

    inline uint64_t pseudolegal_knight_moves(uint8_t square, Board &board){
        uint64_t friendly_pieces = board.white_to_move ? board.white_pieces() : board.black_pieces();

        uint64_t attacks = knight_masks[square] & ~friendly_pieces;
        return attacks;
    }
    uint64_t pseudolegal_rook_moves(uint8_t square, Board &board);
    uint64_t pseudolegal_bishop_moves(uint8_t square, Board &board);
    inline uint64_t pseudolegal_queen_moves(uint8_t square, Board &board){
        return pseudolegal_rook_moves(square, board) | pseudolegal_bishop_moves(square, board);
    }
    uint64_t pseudolegal_king_moves(uint8_t square, Board &board);
    PawnMoves pseudolegal_pawn_moves(Board &board);
    void generate_all_moves(MoveList &list, Board &board);
}