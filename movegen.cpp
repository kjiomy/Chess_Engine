#include "movegen.hpp"

namespace MoveGen{
    uint64_t pseudolegal_knight_moves(uint8_t square, Board &board){
        uint64_t k = 1ULL << square;
        uint64_t friendly_pieces = board.white_to_move ? board.white_pieces() : board.black_pieces();

        uint64_t attacks = (k << 17) & NOT_A_FILE;
        attacks |= (k << 15) & NOT_H_FILE;
        attacks |= (k << 10) & NOT_AB_FILE;
        attacks |= (k <<  6) & NOT_GH_FILE;
        attacks |= (k >> 17) & NOT_H_FILE;
        attacks |= (k >> 15) & NOT_A_FILE;
        attacks |= (k >> 10) & NOT_GH_FILE;
        attacks |= (k >>  6) & NOT_AB_FILE;
        attacks = attacks & ~friendly_pieces;

        return attacks;
    }

    uint64_t pseudolegal_rook_moves(uint8_t square, Board &board){
        uint64_t attacks = 0;
        uint64_t empty = board.empty_squares();
        uint64_t enemy = board.white_to_move ? board.black_pieces() : board.white_pieces();
        uint64_t friendly = board.white_to_move ? board.white_pieces() : board.black_pieces();

        int row = square / 8;
        int column = square % 8;

        int dr[] = {1, -1, 0, 0};
        int dc[] = {0, 0, 1, -1};

        for(int i = 0; i < 4; i++){
            for(int step = 1; step < 8; step++){
                int next_r = row + dr[i] * step;
                int next_c = column + dc[i] * step;

                if(next_r < 0 || next_r > 7 || next_c  < 0|| next_c > 7) break;

                int next_square = next_r * 8 + next_c;
                uint64_t bit = 1ULL << next_square;

                if (bit & friendly) break;
                attacks |= bit;
                if (bit & enemy) break;
            }
        }

        return attacks;
    }

    uint64_t pseudolegal_bishop_moves(uint8_t square, Board &board){
        uint64_t attacks = 0;
        uint64_t enemy = board.white_to_move ? board.black_pieces() : board.white_pieces();
        uint64_t friendly = board.white_to_move ? board.white_pieces() : board.black_pieces();

        int row = square / 8;
        int column = square % 8;

        int dr[] = {1, 1, -1, -1};
        int dc[] = {1, -1, 1, -1};

        for(int i = 0; i < 4; i++){
            for(int step = 1; step < 8; step++){
                int next_r = row + dr[i] * step;
                int next_c = column + dc[i] * step;

                if(next_r < 0 || next_r > 7 || next_c  < 0|| next_c > 7) break;

                int next_square = next_r * 8 + next_c;
                uint64_t bit = 1ULL << next_square;

                if (bit & friendly) break;
                attacks |= bit;
                if (bit & enemy) break;
            }
        }

        return attacks;
    }

    uint64_t pseudolegal_king_moves(uint8_t square, Board &board){
        uint64_t k = 1ULL << square;
        uint64_t friendly_pieces = board.white_to_move ? board.white_pieces() : board.black_pieces();
        uint64_t occupied_squares = board.all_pieces();

        uint64_t attacks = (k << 1) & NOT_A_FILE;
        attacks |= (k >> 1) & NOT_H_FILE;
        attacks |= (k << 7) & NOT_H_FILE;
        attacks |= (k << 8);
        attacks |= (k << 9) & NOT_A_FILE;
        attacks |= (k >> 7) & NOT_A_FILE;
        attacks |= (k >> 8);
        attacks |= (k >> 9) & NOT_H_FILE;

        attacks &= ~friendly_pieces;

        // Castling logic
        if(board.white_to_move){
            // White side
            if(square == 4){
                // King side
                if((board.castling_rights & WK) && !(occupied_squares & (1ULL  << 5 | 1ULL << 6))){
                    attacks |= (1ULL << 6);
                }

                // Queen side
                if((board.castling_rights & WQ) && !(occupied_squares & (1ULL  << 3 | 1ULL << 2 | 1ULL << 1))){
                    attacks |= (1ULL << 2);
                }
            }
        }else {
            // Black side
            if(square == 60){
                // King side
                if((board.castling_rights & BK) && !(occupied_squares &(1ULL << 61 | 1ULL << 62))){
                    attacks |= (1ULL << 62);
                }

                if((board.castling_rights & BQ) && !(occupied_squares &(1ULL << 59 | 1ULL << 58 | 1ULL << 57))){
                    attacks |= (1ULL << 58);
                }
            }
        }


        return attacks;
    }

    PawnMoves pseudolegal_pawn_moves(Board &board){
        PawnMoves moves;
        
        if(board.white_to_move){
            uint64_t empty = board.empty_squares();
            uint64_t enemy = board.black_pieces() | board.en_passant_target;


            moves.single_push = (board.white_pawns << 8) & empty;
            moves.double_push = ((moves.single_push & 0x0000000000FF0000) << 8) & empty;

            moves.capture_left = (board.white_pawns << 7) & NOT_H_FILE & enemy;
            moves.capture_right = (board.white_pawns << 9) & NOT_A_FILE & enemy;
        return moves;
        }else {
            uint64_t empty = board.empty_squares();
            uint64_t enemy = board.white_pieces() | board.en_passant_target;


            moves.single_push = (board.black_pawns >> 8) & empty;
            moves.double_push = ((moves.single_push & 0x0000FF0000000000) >> 8) & empty;

            moves.capture_left = (board.black_pawns >> 7) & NOT_A_FILE & enemy;
            moves.capture_right = (board.black_pawns >> 9) & NOT_H_FILE & enemy;
            return moves;
        }
    }

    void generate_all_moves(MoveList &list, Board &board){
        uint64_t enemy_pieces = board.white_to_move ? board.black_pieces() : board.white_pieces();

        uint64_t knights = board.white_to_move ? board.white_knights : board.black_knights;
        while(knights){
            int from = pop_lsb(knights);
            uint64_t attacks = MoveGen::pseudolegal_knight_moves(from, board);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                list.push(encode_move(from, to, flag));
            }
        }
    
        uint64_t bishops = board.white_to_move ? board.white_bishops : board.black_bishops;
        while(bishops){
            int from = pop_lsb(bishops);
            uint64_t attacks = MoveGen::pseudolegal_bishop_moves(from, board);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                list.push(encode_move(from, to, flag));
            }
        }
    
        uint64_t rooks = board.white_to_move ? board.white_rooks : board.black_rooks;
        while(rooks){
            int from = pop_lsb(rooks);
            uint64_t attacks = MoveGen::pseudolegal_rook_moves(from, board);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                list.push(encode_move(from, to, flag));
            }
        }

        uint64_t queens = board.white_to_move ? board.white_queens : board.black_queens;
        while(queens){
            int from = pop_lsb(queens);
            uint64_t attacks = MoveGen::pseudolegal_queen_moves(from, board);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                list.push(encode_move(from, to, flag));
            }
        }

        PawnMoves pawn_moves = pseudolegal_pawn_moves(board);
        while(pawn_moves.single_push){
            int to = pop_lsb(pawn_moves.single_push);
            int from = board.white_to_move ? to - 8 : to + 8;
            uint64_t rank_mask = board.white_to_move ? MASK_8_RANK : MASK_1_RANK;

            if((1ULL << to) & rank_mask){
                list.push(encode_move(from, to, PROMO_KNIGHT));
                list.push(encode_move(from, to, PROMO_BISHOP));
                list.push(encode_move(from, to, PROMO_ROOK));
                list.push(encode_move(from, to, PROMO_QUEEN));
            }else{
                list.push(encode_move(from, to, QUIET_MOVE));
            }   
        }

        while(pawn_moves.double_push){
            int to = pop_lsb(pawn_moves.double_push);
            int from = board.white_to_move ? to - 16 : to + 16;

            list.push(encode_move(from, to, DOUBLE_PAWN_PUSH));
        }

        while(pawn_moves.capture_left){
            int to = pop_lsb(pawn_moves.capture_left);
            int from = board.white_to_move ? to - 7 : to + 7;
            int flag = (1ULL << to) & board.en_passant_target ? EP_CAPTURE : CAPTURE;
            uint64_t rank_mask = board.white_to_move ? MASK_8_RANK : MASK_1_RANK;

            if((1ULL << to) & rank_mask){
                list.push(encode_move(from, to, PROMO_CAPTURE_KNIGHT));
                list.push(encode_move(from, to, PROMO_CAPTURE_BISHOP));
                list.push(encode_move(from, to, PROMO_CAPTURE_ROOK));
                list.push(encode_move(from, to, PROMO_CAPTURE_QUEEN));
            }else{
                list.push(encode_move(from, to, flag));
            }
        }

        while(pawn_moves.capture_right){
            int to = pop_lsb(pawn_moves.capture_right);
            int from = board.white_to_move ? to - 9 : to + 9;
            int flag = ((1ULL << to) & board.en_passant_target) ? EP_CAPTURE : CAPTURE;

            uint64_t rank_mask = board.white_to_move ? MASK_8_RANK : MASK_1_RANK;

            if((1ULL << to) & rank_mask){
                list.push(encode_move(from, to, PROMO_CAPTURE_KNIGHT));
                list.push(encode_move(from, to, PROMO_CAPTURE_BISHOP));
                list.push(encode_move(from, to, PROMO_CAPTURE_ROOK));
                list.push(encode_move(from, to, PROMO_CAPTURE_QUEEN));
            }else{
                list.push(encode_move(from, to, flag));
            }
        }
    
        uint64_t king = board.white_to_move ? board.white_king : board.black_king;
        while(king){
            int from = pop_lsb(king);
            uint64_t attacks = MoveGen::pseudolegal_king_moves(from, board);

            while(attacks){
                int to = pop_lsb(attacks);
                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;
                
                bool can_castle = true;

                if(board.white_to_move){
                    if(from == 4){
                        if(to == 2) {
                            flag = QUEEN_CASTLE;
                            if(board.is_square_attacked(4, false) || board.is_square_attacked(3, false)) can_castle = false;
                        }
                        if(to == 6) {
                            flag = KING_CASTLE;
                            if(board.is_square_attacked(4, false) || board.is_square_attacked(5, false)) can_castle = false;
                        }
                    }
                }else{
                    if(from == 60){
                        if(to == 58) {
                            flag = QUEEN_CASTLE;
                            if(board.is_square_attacked(60, true) || board.is_square_attacked(59, true)) can_castle = false;
                        }
                        if(to == 62) {
                            flag = KING_CASTLE;
                            if(board.is_square_attacked(60, true) || board.is_square_attacked(61, true)) can_castle = false;
                        }
                    }
                }

                if (can_castle) {
                    list.push(encode_move(from, to, flag));
                }
            }
        }
    }
}