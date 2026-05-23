#include <iostream>
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

        int score = -get_alphabeta(board, depth - 1, -beta, -alpha, 0);
        board.unmake_move(m, prev_state);

        if(score > alpha) { 
            alpha = score;
            best_move = m;
        }
    }

    record_tt(board.hash_key, depth, alpha, TT_EXACT, best_move);

    std::cout << "info depth " << depth 
              << " nodes " << node_searched << std::endl;
    node_searched = 0;

    return best_move;
}


    int score_move(Board &board, Move m){
        int flag = get_move_flags(m);
        int score = 0;

        PieceType attacker, attacked;

        if(flag >= CAPTURE){
            int from = get_move_from(m);
            int to = get_move_to(m);

            if(flag == EP_CAPTURE){
                to = board.white_to_move ? to - 8 : to + 8;
            }

            attacker = board.get_piece_at(from);
            attacked = board.get_piece_at(to);

            score += (10 * Eval::piece_value[attacked]) - Eval::piece_value[attacker];
        }

        if(flag >= PROMO_KNIGHT){
            if (flag == PROMO_QUEEN || flag == PROMO_CAPTURE_QUEEN) {
                score += 10 * Eval::piece_value[QUEEN];
            } 
            else if (flag == PROMO_KNIGHT || flag == PROMO_CAPTURE_KNIGHT) {
                score += 10 * Eval::piece_value[KNIGHT];
            } 
            else if (flag == PROMO_ROOK || flag == PROMO_CAPTURE_ROOK) {
                score += 10 * Eval::piece_value[ROOK];
            } 
            else if (flag == PROMO_BISHOP || flag == PROMO_CAPTURE_BISHOP) {
                score += 10 * Eval::piece_value[BISHOP];
            }
        }


        return score;
    }

    void sort_moves(MoveList &list, Board &board, Move hash_move){
        int scores[256];

        for (int i = 0; i < list.count; i++) {
            if (list.moves[i] == hash_move) {
                scores[i] = 10000000; 
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

    static inline int quiescence_search(Board &board, int alpha, int beta, int ply){
        int stand_pat = Eval::evaluate(board);

        if (stand_pat >= beta) return beta;
        if (stand_pat > alpha) return stand_pat;

        MoveList list;
        MoveGen::generate_all_moves(list, board);
        sort_moves(list, board, 0);

        for(int i = 0; i < list.count; i++){
            Move m = list.moves[i];
            int flag = get_move_flags(m);

            bool is_capture = (flag == CAPTURE || flag == EP_CAPTURE || (flag >= PROMO_CAPTURE_KNIGHT && flag <= PROMO_CAPTURE_QUEEN));
            bool is_promo = (flag >= PROMO_KNIGHT && flag <= PROMO_CAPTURE_QUEEN);

            if(!is_promo && !is_capture) continue;

            if (is_capture && !is_promo) {
                int to = get_move_to(m);
                PieceType captured = board.get_piece_at(to);
                if (captured != EMPTY) {
                    if (stand_pat + Eval::piece_value[captured] + 200 < alpha) continue;
                }
            }


            BoardState prev_state = board.make_move(m);

            bool moving_side = !board.white_to_move;
            uint64_t my_king = board.bitboards[moving_side][KING];
            if(my_king == 0) {
                board.unmake_move(m, prev_state); 
                continue;
            }

            uint8_t king_square = __builtin_ctzll(my_king);
            if(board.is_square_attacked(king_square, board.white_to_move)){
                board.unmake_move(m, prev_state);
                continue;
            }

            int score = -quiescence_search(board, -beta, -alpha, ply + 1);
            board.unmake_move(m, prev_state);

            if (score >= beta) return beta;
            if (score > alpha) return score;
        }

        return alpha;
    }

    int get_alphabeta(Board &board, int depth, int alpha, int beta, int ply){
        node_searched++;
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

        if(depth == 0) return quiescence_search(board, alpha, beta, ply);

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

            int score = -get_alphabeta(board, depth - 1, -beta, -alpha, ply + 1);
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

        // Seaching for mate

        if (legal_moves == 0) {
            bool current_turn = board.white_to_move;
            uint64_t my_king = board.bitboards[current_turn][KING];
            
            if (my_king != 0) {
                uint8_t king_square = __builtin_ctzll(my_king);
                
                if (board.is_square_attacked(king_square, !current_turn)) {
                    return -1000000 + ply; 
                }
            }
        }

        record_tt(board.hash_key, depth, alpha, tt_flag, best_move);
        return alpha;
    }

}