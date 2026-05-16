#include "evaluation.hpp"

namespace Eval{
    const int piece_value[7] = {0, 100, 300, 320, 500, 900, 0};

    int evaluate(Board &board){
        int white_score = 0;
        int black_score = 0;

        for(int p = PAWN; p <= KING; p++){
            int white_count = __builtin_popcountll(board.bitboards[WHITE][p]);
            int black_count = __builtin_popcountll(board.bitboards[BLACK][p]);

            white_score += white_count * piece_value[p];
            black_score += black_count * piece_value[p];
        }

        int evaluation = white_score - black_score;

        return board.white_to_move ? evaluation : -evaluation;
    }
}