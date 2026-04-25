#include "board.hpp"


    Board::Board(){
        init_board();
    }

    void Board::init_board(){
        white_pawns = 0x000000000000FF00;
        white_rooks = 0x0000000000000081;
        white_bishops = 0x0000000000000024;
        white_knights = 0x0000000000000042;
        white_queens = 0x0000000000000008;
        white_king = 0x0000000000000010;

        black_pawns = 0x00FF000000000000;
        black_rooks = 0x8100000000000000;
        black_bishops = 0x2400000000000000;
        black_knights = 0x4200000000000000;
        black_queen = 0x0800000000000000;
        black_king = 0x1000000000000000;
    }

    uint64_t Board::white_pieces(){
        return white_pawns | white_bishops | white_king | white_knights | white_rooks | white_queens;
    }

    uint64_t Board::black_pieces(){
        return black_pawns | black_rooks | black_bishops | black_knights | black_queen | black_king;
    }

    uint64_t Board::all_pieces(){
        return white_pieces() | black_pieces();
    }

    uint64_t Board::empty_squares(){
        return ~all_pieces();
    }

    void Board::print_binary(uint64_t bitboard){
        std::cout << std::bitset<64>(bitboard) << std::endl;
    }

    void Board::print_bitboard(uint64_t bitboard){
        for(int column = 7; column >= 0; column--){
            std::cout << column + 1 << "  ";

            for(int row = 0; row < 8; row++){
                int square = column * 8 + row;

                if((bitboard >> square) & 1ULL){
                    std::cout << "1 ";
                }else{
                    std::cout << ". ";
                }
            }

            std::cout << std::endl;
        }

        std::cout << std::endl << "  a b c d e f g h" << std::endl;
    }
