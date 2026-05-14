#pragma once
#include <cstdint>
#include "types.hpp"
#include "board.hpp"

namespace MoveGen{
    uint64_t pseudolegal_knight_moves(uint8_t square, Board &board);
    uint64_t pseudolegal_rook_moves(uint8_t square, Board &board);
}