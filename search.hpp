#pragma once
#include "board.hpp"
#include "types.hpp"

namespace Search{
    Move get_best_move(Board &board, int depth);

    int get_minmax_score(Board &board, int depth);

    Move get_greedy_move(Board &board);
}