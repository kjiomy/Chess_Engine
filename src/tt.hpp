#pragma once
#include <stdint.h>
#include "types.hpp"

const int TT_SIZE = 1048576;

enum TTFlag {
    TT_EXACT,
    TT_ALPHA,
    TT_BETA
};

struct TTEntry {
    uint64_t key;
    int score;
    int depth;
    TTFlag flag;
    Move best_move;
};

inline TTEntry TT[TT_SIZE];

inline void record_tt(uint64_t hash_key, int depth, int score, TTFlag flag, Move best_move) {
    int index = hash_key % TT_SIZE;

    if (score > 900000)  score += depth;
    if (score < -900000) score -= depth;

    if (TT[index].key == 0 || TT[index].depth < depth) {  
        TT[index].key = hash_key;
        TT[index].depth = depth;
        TT[index].score = score;
        TT[index].flag = flag;
        TT[index].best_move = best_move;
    }
}