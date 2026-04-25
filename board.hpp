#include <cstdint>

class Board{
    public:
        uint64_t white_pawns;
        uint64_t white_rooks;
        uint64_t white_bishops;
        uint64_t white_knights;
        uint64_t white_queens;
        uint64_t white_king;

        uint64_t black_pawns;
        uint64_t black_rooks;
        uint64_t black_bishops;
        uint64_t black_knights;
        uint64_t black_queen;
        uint64_t black_king;

        Board();
        void init_board();
        uint64_t white_pieces();
        uint64_t black_pieces();
        uint64_t all_pieces();
        uint64_t empty_squares();
        void print_binary(uint64_t);
        void print_bitboard(uint64_t);
};