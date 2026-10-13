#include "evaluation.hpp"

const int pst_pawn[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0};

const int pst_knight[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50};

const int pst_bishop[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20};

const int pst_rook[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 5, 5, 0, 0, 0};

const int pst_queen[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20};

const int pst_king_mg[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20};

const int pst_king_eg[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -30, -20, -10, 0, 0, -10, -20, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -30, 0, 0, 0, 0, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50};

const int *pst_table_mg[8] = {
    nullptr,
    pst_pawn,
    pst_knight,
    pst_bishop,
    pst_rook,
    pst_queen,
    pst_king_mg};

const int *pst_table_eg[7] = {
    nullptr,
    pst_pawn,
    pst_knight,
    pst_bishop,
    pst_rook,
    pst_queen,
    pst_king_eg};

namespace Eval
{
    const int phase_weights[7] = {0, 0, 1, 1, 2, 4, 0};

    int evaluate(Board &board)
    {
        int mg_white = 0, mg_black = 0;
        int eg_white = 0, eg_black = 0;
        int game_phase = 0;

        for (int p = PAWN; p <= KING; p++)
        {

            uint64_t white_pieces = board.bitboards[WHITE][p];
            game_phase += __builtin_popcountll(white_pieces) * phase_weights[p];

            while (white_pieces)
            {
                uint8_t sq = __builtin_ctzll(white_pieces);
                uint8_t target_sq = sq ^ 56;

                mg_white += pst_table_mg[p][target_sq] + piece_value[p];
                eg_white += pst_table_eg[p][target_sq] + piece_value[p];

                white_pieces &= white_pieces - 1;
            }

            uint64_t black_pieces = board.bitboards[BLACK][p];
            game_phase += __builtin_popcountll(black_pieces) * phase_weights[p];

            while (black_pieces)
            {
                uint8_t sq = __builtin_ctzll(black_pieces);
                uint8_t target_sq = sq;

                mg_black += pst_table_mg[p][target_sq] + piece_value[p];
                eg_black += pst_table_eg[p][target_sq] + piece_value[p];

                black_pieces &= black_pieces - 1;
            }
        }

        if (game_phase > 24)
            game_phase = 24;

        int phase = game_phase;

        int mg_score = mg_white - mg_black;
        int eg_score = eg_white - eg_black;

        int evaluation = ((mg_score * phase) + (eg_score * (24 - phase))) / 24;

        return board.white_to_move ? evaluation : -evaluation;
    }
}