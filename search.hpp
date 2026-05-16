#pragma once
#include "board.hpp"
#include "types.hpp"

namespace Search{
    Move get_best_move(Board &board, int depth);

    int get_minmax_score(Board &board, int depth);

    int get_alphabeta(Board &board, int depth, int alpha, int beta);

    Move get_greedy_move(Board &board);
}