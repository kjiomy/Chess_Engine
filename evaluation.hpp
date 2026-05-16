#pragma once
#include "board.hpp"
#include "types.hpp"


namespace Eval {
    int evaluate(Board &board);

    int get_pst_value(int square, bool is_white, int piece);
}


