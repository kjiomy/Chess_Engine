#pragma once
#include <cstdint>
#include "types.hpp"
#include "board.hpp"

namespace MoveGen{
    uint64_t pseudolegal_knight_moves(uint8_t square, Board &board);
    uint64_t pseudolegal_rook_moves(uint8_t square, Board &board);
    uint64_t pseudolegal_bishop_moves(uint8_t square, Board &board);
    inline uint64_t pseudolegal_queen_moves(uint8_t square, Board &board){
        return pseudolegal_rook_moves(square, board) | pseudolegal_bishop_moves(square, board);
    }
    uint64_t pseudolegal_king_moves(uint8_t square, Board &board);
    PawnMoves pseudolegal_pawn_moves(Board &board);
}