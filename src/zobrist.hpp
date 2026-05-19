#pragma once
#include <cstdint>
#include "board.hpp"

namespace Zobrist{
    inline uint64_t piece_keys[2][7][64];
    inline uint64_t side_key;
    inline uint64_t castle_key[16];
    inline uint64_t en_passant_key[8];

    void init();
    uint64_t generate_hash(const Board &board);
}