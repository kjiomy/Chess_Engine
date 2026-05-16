#include "search.hpp"
#include "movegen.hpp"
#include "evaluation.hpp"

namespace Search{
    Move get_best_move(Board &board, int depth){
        return 0;
    }

    Move get_greedy_move(Board &board){
        MoveList moves;
        MoveGen::generate_all_moves(moves, board);

        int best_score = -10000000;
        Move best_move = 0;

        for(int i = 0; i < moves.count; i++){
            Move m = moves.moves[i];
            BoardState state = board.make_move(m);

            bool moving_side = !board.white_to_move;
            uint64_t king = board.bitboards[moving_side][KING];

            if(king != 0){
                uint8_t king_square = __builtin_ctzll(king);

                if(!board.is_square_attacked(king_square, board.white_to_move)){
                    int score = -Eval::evaluate(board);

                    if(score > best_score){
                        best_score = score;
                        best_move = m;
                    }
                }
            }   

            board.unmake_move(m, state);
        }

        return best_move;
    }
}