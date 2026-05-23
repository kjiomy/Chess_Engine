#pragma once
#include "board.hpp"
#include "types.hpp"

namespace Search{
    inline int node_searched = 0;

    Move iterative_deepening(Board &board, int engine_time);

    Move get_best_move(Board &board, int depth);

    int get_alphabeta(Board &board, int depth, int alpha, int beta, int ply);
}