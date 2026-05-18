#include "board.hpp"
#include "movegen.hpp"
#include <iostream>
#include <bitset>
#include <algorithm>



    void Board::print_board(){
        std::vector<std::string> board(64, ".");
        std::string black_unicode[] = {".", "♙", "♘", "♗", "♖", "♕", "♔"};
        std::string white_unicode[] = {".", "♟", "♞", "♝", "♜", "♛", "♚"};

        uint64_t whites = white_pieces();
        uint64_t blacks = black_pieces();

        while(whites){
            uint8_t current = pop_lsb(whites);

            board[current] = white_unicode[get_piece_at(current)];
        }

        while(blacks){
            uint8_t current = pop_lsb(blacks);

            board[current] = black_unicode[get_piece_at(current)];
        }
        

        for(int row = 7; row >= 0; row--){
            std::cout << row + 1 << "  ";

            for(int column = 0; column < 8; column++){
                std::cout << board[row * 8 + column] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << std::endl << "   a b c d e f g h" << std::endl;
    }

    Board::Board(){
        init_board();
    }

    void Board::init_board(){
        bitboards[WHITE][EMPTY] = 0; 
        bitboards[WHITE][PAWN] = 0x000000000000FF00;
        bitboards[WHITE][KNIGHT] = 0x0000000000000042;
        bitboards[WHITE][BISHOP] = 0x0000000000000024;
        bitboards[WHITE][ROOK] = 0x0000000000000081;
        bitboards[WHITE][QUEEN] = 0x0000000000000008;
        bitboards[WHITE][KING] = 0x0000000000000010;

        bitboards[BLACK][EMPTY] = 0; 
        bitboards[BLACK][PAWN] = 0x00FF000000000000;
        bitboards[BLACK][KNIGHT] = 0x4200000000000000;
        bitboards[BLACK][BISHOP] = 0x2400000000000000;
        bitboards[BLACK][ROOK] = 0x8100000000000000;
        bitboards[BLACK][QUEEN] = 0x0800000000000000;
        bitboards[BLACK][KING] = 0x1000000000000000;
        
        std::fill_n(piece_list, 64, EMPTY);
        for (int color = 0; color < 2; color++) {
            for (int piece = PAWN; piece <= KING; piece++) {
                uint64_t bb = bitboards[color][piece];
                while (bb) {
                    int square = pop_lsb(bb);
                    piece_list[square] = static_cast<PieceType>(piece);
                }
            }
        }

        castling_rights = 0x0F;
        en_passant_target = 0x0000000000000000;
        white_to_move = true;
    }


   
    bool Board::is_square_attacked(uint8_t square, bool is_white){

        bool original_turn = white_to_move;
    
        white_to_move = !is_white;

        uint64_t enemy_knights = bitboards[is_white][KNIGHT];
        uint64_t enemy_bishops = bitboards[is_white][BISHOP];
        uint64_t enemy_rooks   = bitboards[is_white][ROOK];
        uint64_t enemy_queens  = bitboards[is_white][QUEEN];
        uint64_t enemy_king    = bitboards[is_white][KING];
        uint64_t enemy_pawns   = bitboards[is_white][PAWN];

        bool attacked = false;

        if (MoveGen::pseudolegal_knight_moves(square, *this) & enemy_knights) attacked = true;
        else if (MoveGen::pseudolegal_bishop_moves(square, *this) & (enemy_bishops | enemy_queens)) attacked = true;
        else if (MoveGen::pseudolegal_rook_moves(square, *this) & (enemy_rooks | enemy_queens)) attacked = true;
        else if (MoveGen::pseudolegal_king_moves(square, *this) & enemy_king) attacked = true;


        uint64_t sq_bit = 1ULL << square;
        if (!attacked) {
            if (is_white) {
                if ((sq_bit >> 7) & NOT_A_FILE & enemy_pawns) attacked = true;
                if ((sq_bit >> 9) & NOT_H_FILE & enemy_pawns) attacked = true;
            } else {
                if ((sq_bit << 7) & NOT_H_FILE & enemy_pawns) attacked = true;
                if ((sq_bit << 9) & NOT_A_FILE & enemy_pawns) attacked = true;
            }
        }


        white_to_move = original_turn;

        return attacked;
    }
   

    BoardState Board::make_move(Move move){
        int flags = get_move_flags(move);
        int from = get_move_from(move);
        int to = get_move_to(move);

        PieceType moved_piece = get_piece_at(from);
        PieceType captured_piece;
        int captured_square = to;

        if(flags == EP_CAPTURE){
            captured_square = white_to_move ? to - 8 : to + 8;
            captured_piece = PAWN;
        }else{
            captured_piece  = get_piece_at(to);
        }

        BoardState current_state = {castling_rights, en_passant_target, captured_square, captured_piece, !white_to_move};
        

        en_passant_target = 0;

        // Implementing all the flags...

        uint64_t move_mask;

        switch(flags){
            case QUIET_MOVE:
                move_piece(from, to, white_to_move, moved_piece);
                break;

            case DOUBLE_PAWN_PUSH:
                en_passant_target = white_to_move ? (1ULL << (from + 8)) : (1ULL << (from - 8));
                move_piece(from, to, white_to_move, moved_piece);
                break;
            
            case KING_CASTLE:
                if(white_to_move){
                    move_piece(from, to, white_to_move, moved_piece); 
                    move_piece(7, 5, white_to_move, ROOK);            
                    castling_rights &= ~(WK | WQ);
                }else{
                    move_piece(from, to, white_to_move, moved_piece); 
                    move_piece(63, 61, white_to_move, ROOK);          
                    castling_rights &= ~(BK | BQ);
                }
                break;

            case QUEEN_CASTLE:
                if(white_to_move){
                    move_piece(from, to, white_to_move, moved_piece); 
                    move_piece(0, 3, white_to_move, ROOK);            
                    castling_rights &= ~(WK | WQ);
                }else{
                    move_piece(from, to, white_to_move, moved_piece); 
                    move_piece(56, 59, white_to_move, ROOK);          
                    castling_rights &= ~(BK | BQ);
                }
                break;

            case CAPTURE:
                remove_piece(captured_square, captured_piece, !white_to_move);
                move_piece(from, to, white_to_move, moved_piece);
                break;

            case EP_CAPTURE:
                remove_piece(captured_square, captured_piece, !white_to_move);
                move_piece(from, to, white_to_move, moved_piece);
                break;

            case PROMO_KNIGHT:
                remove_piece(from, PAWN, white_to_move);
                place_piece(to, KNIGHT, white_to_move);
                break;

            case PROMO_BISHOP:
                remove_piece(from, PAWN, white_to_move);
                place_piece(to, BISHOP, white_to_move);
                break;
            
            case PROMO_ROOK:
                remove_piece(from, PAWN, white_to_move);
                place_piece(to, ROOK, white_to_move);
                break;
            
            case PROMO_QUEEN:
                remove_piece(from, PAWN, white_to_move);
                place_piece(to, QUEEN, white_to_move);
                break;

            case PROMO_CAPTURE_KNIGHT:
                remove_piece(from, PAWN, white_to_move);
                remove_piece(captured_square, captured_piece, !white_to_move);
                place_piece(to, KNIGHT, white_to_move);
                break;

            case PROMO_CAPTURE_BISHOP:
                remove_piece(from, PAWN, white_to_move);
                remove_piece(captured_square, captured_piece, !white_to_move);
                place_piece(to, BISHOP, white_to_move);
                break;

            case PROMO_CAPTURE_ROOK:
                remove_piece(from, PAWN, white_to_move);
                remove_piece(captured_square, captured_piece, !white_to_move);
                place_piece(to, ROOK, white_to_move);
                break;

            case PROMO_CAPTURE_QUEEN:
                remove_piece(from, PAWN, white_to_move);
                remove_piece(captured_square, captured_piece, !white_to_move);
                place_piece(to, QUEEN, white_to_move);
                break;
        }

        // Updating castling rights

        if(from == 0 || to == 0) castling_rights &= ~WQ;
        if(from == 7 || to == 7) castling_rights &= ~WK;
        if(from == 56 || to == 56) castling_rights &= ~BQ;
        if(from == 63 || to == 63) castling_rights &= ~BK;
        if(from == 4) castling_rights &= ~(WK | WQ);
        if(from == 60) castling_rights &= ~(BK | BQ);

        white_to_move = !white_to_move;

        return current_state;
    }

    void Board::unmake_move(Move move, BoardState prev_state){

        castling_rights = prev_state.castling_rights;
        en_passant_target = prev_state.en_passant_target;
        white_to_move = !white_to_move;

        int flag = get_move_flags(move);
        int from = get_move_from(move);
        int to = get_move_to(move);

        PieceType moved_piece = get_piece_at(to);

        if(flag >= PROMO_KNIGHT){
            moved_piece = PAWN;
            
            remove_piece(to, get_piece_at(to), white_to_move);
            place_piece(from, PAWN, white_to_move);
        }else{
            uint64_t move_mask = (1ULL << from) | (1ULL << to);
            move_piece(to, from, white_to_move, moved_piece);
        }

        if(prev_state.captured_piece != EMPTY){
            place_piece(prev_state.captured_square, prev_state.captured_piece, prev_state.piece_was_white);
        }

        if (flag == KING_CASTLE) {
            if (white_to_move) { 
                move_piece(5, 7, white_to_move, ROOK); 
            } else {
                move_piece(61, 63, white_to_move, ROOK); 
            }
        } else if (flag == QUEEN_CASTLE) {
            if (white_to_move) {
                move_piece(3, 0, white_to_move, ROOK); 
            } else {
                move_piece(59, 56, white_to_move, ROOK); 
            }
        }
    }

