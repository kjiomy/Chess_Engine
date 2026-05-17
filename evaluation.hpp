#pragma once
#include "board.hpp"
#include "types.hpp"


namespace Eval {
    const int piece_value[7] = {0, 100, 300, 320, 500, 900, 0};

    int evaluate(Board &board);

    int get_pst_value(int square, bool is_white, int piece);
}


