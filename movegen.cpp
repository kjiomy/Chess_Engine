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
}