#include "search.hpp"
#include "movegen.hpp"
#include "evaluation.hpp"

namespace Search{
    Move get_best_move(Board &board, int depth) {
    MoveList list;
    MoveGen::generate_all_moves(list, board);
    Move best_move = 0;
    int best_score = -10000000;

    for(int i = 0; i < list.count; i++) {
        Move m = list.moves[i];
        BoardState prev_state = board.make_move(m);

        bool moving_side = !board.white_to_move;
        uint64_t king = board.bitboards[moving_side][KING];
        if(king != 0) {
            uint8_t king_square = __builtin_ctzll(king);
            if(board.is_square_attacked(king_square, board.white_to_move)) {
                board.unmake_move(m, prev_state);
                continue;
            }
        }

        int score = -get_minmax_score(board, depth - 1);
        board.unmake_move(m, prev_state);

        if(score > best_score || best_move == 0) {
            best_score = score;
            best_move = m;  
        }
    }

    return best_move;
}

int get_minmax_score(Board &board, int depth) {
    if(depth == 0) return Eval::evaluate(board);
    
    MoveList list;
    MoveGen::generate_all_moves(list, board);

    int best_score = -10000000;
    int legal_moves = 0;

    for(int i = 0; i < list.count; i++) {
        Move m = list.moves[i];
        BoardState prev_state = board.make_move(m);

        bool moving_side = !board.white_to_move;
        uint64_t king = board.bitboards[moving_side][KING];

        if(king != 0) {
            uint8_t king_square = __builtin_ctzll(king);
            if(board.is_square_attacked(king_square, board.white_to_move)) {
                board.unmake_move(m, prev_state);
                continue;
            }
        } 

        legal_moves++;

        int score = -get_minmax_score(board, depth - 1);
        board.unmake_move(m, prev_state);

        if(score > best_score) best_score = score;
    }

    if (legal_moves == 0) {
        bool current_turn = board.white_to_move;
        uint64_t my_king = board.bitboards[current_turn][KING];
        if (my_king != 0) {
            uint8_t king_square = __builtin_ctzll(my_king);
            if (board.is_square_attacked(king_square, !current_turn)) {
                return -1000000 + (10 - depth);
            }
        }
        return 0;
    }

    return best_score;
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