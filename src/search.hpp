#pragma once
#include "board.hpp"
#include "types.hpp"

namespace Search{
    inline int node_searched = 0;
    inline uint64_t position_history[1024];
    inline int history_count = 0;
    inline bool stop_search = false;

    Move iterative_deepening(Board &board, int engine_time, int engine_inc);

    Move get_best_move(Board &board, int depth, Move previous_best);

    int get_alphabeta(Board &board, int depth, int alpha, int beta, int ply);
}