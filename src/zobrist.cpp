#include "zobrist.hpp"
#include <random>

namespace Zobrist{
    void init(){
        std::mt19937_64 rng(1000057409ULL);

        for(int color = 0; color < 2; color++){
            for(int type = 0; type < 7; type++){
                for(int square = 0; square < 64; square++){
                    piece_keys[color][type][square] = rng();
                }
            }
        }

        side_key = rng();

        for(int i = 0; i < 16; i++){
            castle_key[i] = rng();
        }

        for(int i = 0; i < 8; i++){
            en_passant_key[i] = rng();
        }
    }

    uint64_t generate_hash(const Board &board){
        uint64_t final_key = 0;

        for(int square = 0; square < 64; square++){
            PieceType piece = board.get_piece_at(square);
            if(piece != EMPTY){
                uint64_t color_mask = 1ULL << square;

                int color = (color_mask & board.bitboards[WHITE][piece]) ? WHITE : BLACK;
                final_key ^= piece_keys[color][piece][square];
            }
        }
        if (board.white_to_move) final_key ^= side_key;
        final_key ^= castle_key[board.castling_rights];
        
        if(board.en_passant_target != 0) {
            int ep_square = __builtin_ctzll(board.en_passant_target);
            int ep_file = ep_square % 8;
            final_key ^= en_passant_key[ep_file];
        }
        return final_key;
    }
}