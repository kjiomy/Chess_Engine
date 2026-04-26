#include "board.hpp"
#include <iostream>
#include <bitset>


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
        black_queens = 0x0800000000000000;
        black_king = 0x1000000000000000;

        white_to_move = true;
    }

    uint64_t Board::white_pieces(){
        return white_pawns | white_bishops | white_king | white_knights | white_rooks | white_queens;
    }

    uint64_t Board::black_pieces(){
        return black_pawns | black_rooks | black_bishops | black_knights | black_queens | black_king;
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

        std::cout << std::endl << "   a b c d e f g h" << std::endl;
    }

    uint64_t Board::knight_attacks(uint64_t square, uint64_t friendly_pieces){
        uint64_t k = 1ULL << square;

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


    // ATTUALMENTE SENZA EN PASSANT
    PawnMoves Board::white_pawn_moves(){
        PawnMoves moves;
        
        uint64_t empty = empty_squares();
        uint64_t enemy = black_pieces();


        moves.single_push = (white_pawns << 8) & empty;
        moves.double_push = ((moves.single_push & 0x0000000000FF0000) << 8) & empty;

        moves.capture_left = (white_pawns << 7) & NOT_H_FILE & enemy;
        moves.capture_right = (white_pawns << 9) & NOT_A_FILE & enemy;
        return moves;
    }

    PawnMoves Board::black_pawn_moves(){
        PawnMoves moves;
        
        uint64_t empty = empty_squares();
        uint64_t enemy = white_pieces();


        moves.single_push = (black_pawns >> 8) & empty;
        moves.double_push = ((moves.single_push & 0x0000FF0000000000) >> 8) & empty;

        moves.capture_left = (black_pawns >> 7) & NOT_H_FILE & enemy;
        moves.capture_right = (black_pawns >> 9) & NOT_A_FILE & enemy;
        return moves;
    }
