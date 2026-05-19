#include "search.hpp"
#include "movegen.hpp"
#include "evaluation.hpp"
#include "tt.hpp"
#include "types.hpp"


namespace Search{
    Move get_best_move(Board &board, int depth) {
    MoveList list;
    MoveGen::generate_all_moves(list, board);
    Move best_move = 0;
    int alpha = -10000000;
    int beta = 10000000;

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
        } else {
            board.unmake_move(m, prev_state);
            continue;
        }


        if(best_move == 0) best_move = m;

        int score = -get_alphabeta(board, depth - 1, -beta, -alpha);
        board.unmake_move(m, prev_state);

        if(score > alpha) { 
            alpha = score;
            best_move = m;
        }
    }

    // Bug 3 fix: salva il risultato della root nella TT
    record_tt(board.hash_key, depth, alpha, TT_EXACT, best_move);

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

    int score_move(Board &board, Move m){
        int from = get_move_from(m);
        int to = get_move_to(m);

        int attacker = board.get_piece_at(from);
        int attacked = board.get_piece_at(to);

        if(attacked != 0){
            return (10 * Eval::piece_value[attacked]) - Eval::piece_value[attacker];
        }

        return 0;
    }

    void sort_moves(MoveList &list, Board &board, Move hash_move){
        int scores[256];

        for (int i = 0; i < list.count; i++) {
            if (list.moves[i] == hash_move) {
                scores[i] = 10000000; // Punteggio infinito, andrà in cima!
            } else {
                scores[i] = score_move(board, list.moves[i]);
            }
        }

        for (int i = 0; i < list.count - 1; i++) {
            int best_idx = i;
            
            for (int j = i + 1; j < list.count; j++) {
                if (scores[j] > scores[best_idx]) {
                    best_idx = j;
                }
            }

            int temp_score = scores[i];
            scores[i] = scores[best_idx];
            scores[best_idx] = temp_score;

            Move temp_move = list.moves[i];
            list.moves[i] = list.moves[best_idx];
            list.moves[best_idx] = temp_move;
        }
    }

    int get_alphabeta(Board &board, int depth, int alpha, int beta){
        int tt_index = board.hash_key % TT_SIZE;
        TTEntry entry = TT[tt_index];
        Move hash_move = 0;

        if (entry.key == board.hash_key) {
            hash_move = entry.best_move;
            
            if (entry.depth >= depth) {
                int tt_score = entry.score;

                if (tt_score > 900000)  tt_score -= depth;
                if (tt_score < -900000) tt_score += depth;

                if (entry.flag == TT_EXACT) return tt_score;
                if (entry.flag == TT_ALPHA && tt_score <= alpha) return alpha;
                if (entry.flag == TT_BETA  && tt_score >= beta)  return beta;
            }
        }

        if(depth == 0) return Eval::evaluate(board);

        MoveList list;
        MoveGen::generate_all_moves(list, board);
        sort_moves(list, board, hash_move);

        int legal_moves = 0;
        
        TTFlag tt_flag = TT_ALPHA; 
        Move best_move = 0;

        for(int i = 0; i < list.count; i++){
            Move m = list.moves[i];
            BoardState prev_state = board.make_move(m);

            bool moving_side = !board.white_to_move;
            uint64_t king = board.bitboards[moving_side][KING];

            if(king != 0){
                uint8_t sq = __builtin_ctzll(king);
                if(board.is_square_attacked(sq, board.white_to_move)){
                    board.unmake_move(m, prev_state);
                    continue;
                }
            }else{
                board.unmake_move(m, prev_state);
                continue;
            }

            legal_moves++;

            int score = -get_alphabeta(board, depth - 1, -beta, -alpha);
            board.unmake_move(m, prev_state);

            if(score >= beta) {

                record_tt(board.hash_key, depth, beta, TT_BETA, list.moves[i]);
                return beta;
            }
            if(score > alpha) {
                alpha = score;
                tt_flag = TT_EXACT; 
                best_move = list.moves[i]; 
            }
        }

        if (legal_moves == 0) {
            bool cur = board.white_to_move;
            uint64_t my_king = board.bitboards[cur][KING];
            if (my_king != 0) {
                uint8_t ksq = __builtin_ctzll(my_king);
                if (board.is_square_attacked(ksq, !cur))
                    return -1000000 + (10 - depth);
            }
            return 0; 
        }


        record_tt(board.hash_key, depth, alpha, tt_flag, best_move);
        return alpha;
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