#include "board.hpp"
#include "movegen.hpp"
#include <iostream>
#include <bitset>



    void Board::print_board(){
        std::vector<std::string> board(64, ".");
        std::string black_unicode[] = {".", "♙", "♘", "♗", "♖", "♕", "♔"};
        std::string white_unicode[] = {".", "♟", "♞", "♝", "♜", "♛", "♚"};

        uint64_t whites = white_pieces();
        uint64_t blacks = black_pieces();

        while(whites){
            uint8_t current = pop_lsb(whites);

            board[current] = white_unicode[get_piece_at(current, 1)];
        }

        while(blacks){
            uint8_t current = pop_lsb(blacks);

            board[current] = black_unicode[get_piece_at(current, 0)];
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

        castling_rights = 0x0F;
        en_passant_target = 0x0000000000000000;
        white_to_move = true;
    }

// Ausiliary functions 


    void Board::place_piece(int square, PieceType piece, bool is_white){
        uint64_t place_mask = 1ULL << square;

        if(is_white){
            switch(piece){
                case PAWN: white_pawns |= place_mask; break;
                case KNIGHT: white_knights |= place_mask; break;
                case BISHOP: white_bishops |= place_mask; break;
                case ROOK: white_rooks |= place_mask; break;
                case QUEEN: white_queens |= place_mask; break;
                case KING: white_king |= place_mask; break;
                case EMPTY: break;
            }
        }else{
            switch(piece){
                case PAWN: black_pawns |= place_mask; break;
                case KNIGHT: black_knights |= place_mask; break;
                case BISHOP: black_bishops |= place_mask; break;
                case ROOK: black_rooks |= place_mask; break;
                case QUEEN: black_queens |= place_mask; break;
                case KING: black_king |= place_mask; break;
                case EMPTY: break;
            }
        }
    }

    void Board::remove_piece(int square, PieceType piece, bool is_white){
        uint64_t remove_mask = ~(1ULL << square);

        if(is_white){
            switch(piece){
                case PAWN: white_pawns &= remove_mask; break;
                case KNIGHT: white_knights &= remove_mask; break;
                case BISHOP: white_bishops &= remove_mask; break;
                case ROOK: white_rooks &= remove_mask; break;
                case QUEEN: white_queens &= remove_mask; break;
                case KING: white_king &= remove_mask; break;
                case EMPTY: break;
            }
        }else{
            switch(piece){
                case PAWN: black_pawns &= remove_mask; break;
                case KNIGHT: black_knights &= remove_mask; break;
                case BISHOP: black_bishops &= remove_mask; break;
                case ROOK: black_rooks &= remove_mask; break;
                case QUEEN: black_queens &= remove_mask; break;
                case KING: black_king &= remove_mask; break;
                case EMPTY: break;
            }
        }
    }
    
    void Board::toggle_piece(PieceType piece, bool is_white, uint64_t toggle_mask){
        if(is_white){
            switch(piece){
                case PAWN: white_pawns ^= toggle_mask; break;
                case KNIGHT: white_knights ^= toggle_mask; break;
                case BISHOP: white_bishops ^= toggle_mask; break;
                case ROOK: white_rooks ^= toggle_mask; break;
                case QUEEN: white_queens ^= toggle_mask; break;
                case KING: white_king ^= toggle_mask; break;
                case EMPTY: break;
            }
        }else{
            switch(piece){
                case PAWN: black_pawns ^= toggle_mask; break;
                case KNIGHT: black_knights ^= toggle_mask; break;
                case BISHOP: black_bishops ^= toggle_mask; break;
                case ROOK: black_rooks ^= toggle_mask; break;
                case QUEEN: black_queens ^= toggle_mask; break;
                case KING: black_king ^= toggle_mask; break;
                case EMPTY: break;
            }
        }
    }



    PieceType Board::get_piece_at(uint8_t square, bool check_white){
        uint64_t mask = 1ULL << square;

        if(check_white){
            if(white_pawns & mask) return PAWN;
            if(white_knights & mask) return KNIGHT;
            if(white_bishops & mask) return BISHOP;
            if(white_rooks & mask) return ROOK;
            if(white_queens & mask) return QUEEN;
            if(white_king & mask) return KING;
        }else{
            if(black_pawns & mask) return PAWN;
            if(black_knights & mask) return KNIGHT;
            if(black_bishops & mask) return BISHOP;
            if(black_rooks & mask) return ROOK;
            if(black_queens & mask) return QUEEN;
            if(black_king & mask) return KING;
        }

        return EMPTY;
    }
   
    bool Board::is_square_attacked(uint8_t square, bool is_white){
        // Salviamo di chi è effettivamente il turno
        bool original_turn = white_to_move;
        
        // TRICK: Impostiamo il turno al colore del DIFENSORE (!is_white)
        // Così le funzioni pseudolegali sapranno chi "blocca" i raggi e chi no.
        white_to_move = !is_white;

        uint64_t enemy_knights = is_white ? white_knights : black_knights;
        uint64_t enemy_bishops = is_white ? white_bishops : black_bishops;
        uint64_t enemy_rooks   = is_white ? white_rooks   : black_rooks;
        uint64_t enemy_queens  = is_white ? white_queens  : black_queens;
        uint64_t enemy_king    = is_white ? white_king    : black_king;
        uint64_t enemy_pawns   = is_white ? white_pawns   : black_pawns;

        bool attacked = false;

        // Ora i raggi partiranno dal Re e si fermeranno correttamente sui pezzi nemici!
        if (MoveGen::pseudolegal_knight_moves(square, *this) & enemy_knights) attacked = true;
        else if (MoveGen::pseudolegal_bishop_moves(square, *this) & (enemy_bishops | enemy_queens)) attacked = true;
        else if (MoveGen::pseudolegal_rook_moves(square, *this) & (enemy_rooks | enemy_queens)) attacked = true;
        else if (MoveGen::pseudolegal_king_moves(square, *this) & enemy_king) attacked = true;

        // Logica manuale dei pedoni (non è influenzata dal trick, quindi è sicura)
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

        // Ripristiniamo il turno originale per non corrompere la partita
        white_to_move = original_turn;

        return attacked;
    }
   

    
    //da finire con tutti i vari flag ecc...
    BoardState Board::make_move(Move move){
        int flags = get_move_flags(move);
        int from = get_move_from(move);
        int to = get_move_to(move);

        PieceType moved_piece = get_piece_at(from, white_to_move);
        PieceType captured_piece;
        int captured_square = to;

        if(flags == EP_CAPTURE){
            captured_square = white_to_move ? to - 8 : to + 8;
            captured_piece = PAWN;
        }else{
            captured_piece  = get_piece_at(to, !white_to_move);
        }

        BoardState current_state = {castling_rights, en_passant_target, captured_square, captured_piece, !white_to_move};
        

        en_passant_target = 0;

        // Implementing all the flags...

        uint64_t move_mask;

        switch(flags){
            case QUIET_MOVE:
                move_mask = (1ULL << from) |  (1ULL << to);
                toggle_piece(moved_piece, white_to_move, move_mask);
                break;

            case DOUBLE_PAWN_PUSH:
                en_passant_target = white_to_move ? (1ULL << (from + 8)) : (1ULL << (from - 8));
                move_mask = (1ULL << from) |  (1ULL << to);
                toggle_piece(moved_piece, white_to_move, move_mask);
                break;
            
            case KING_CASTLE:
                if(white_to_move){
                    move_mask = (1ULL << from) |  (1ULL << to);
                    toggle_piece(moved_piece, white_to_move, move_mask);

                    uint64_t king_castle_mask = 0x00000000000000A0;
                    toggle_piece(ROOK, white_to_move, king_castle_mask);
                    castling_rights = castling_rights &= ~(WK | WQ);
                }else{
                    move_mask = (1ULL << from) |  (1ULL << to);
                    toggle_piece(moved_piece, white_to_move, move_mask);

                    uint64_t king_castle_mask = 0xA000000000000000;
                    toggle_piece(ROOK, white_to_move, king_castle_mask);
                    castling_rights = castling_rights &= ~(BK | BQ);
                }
                break;

            case QUEEN_CASTLE:
                if(white_to_move){
                    move_mask = (1ULL << from) |  (1ULL << to);
                    toggle_piece(moved_piece, white_to_move, move_mask);

                    uint64_t queen_castle_mask = 0x0000000000000009;
                    toggle_piece(ROOK, white_to_move, queen_castle_mask);
                    castling_rights = castling_rights &= ~(WQ | WK);
                }else{
                    move_mask = (1ULL << from) |  (1ULL << to);
                    toggle_piece(moved_piece, white_to_move, move_mask);

                    uint64_t queen_castle_mask = 0x0900000000000000;
                    toggle_piece(ROOK, white_to_move, queen_castle_mask);
                    castling_rights = castling_rights &= ~(BQ | BK);
                }
                break;

            case CAPTURE:
                remove_piece(captured_square, captured_piece, !white_to_move);
                move_mask = (1ULL << from) |  (1ULL << to);
                toggle_piece(moved_piece, white_to_move, move_mask);
                break;

            case EP_CAPTURE:
                remove_piece(captured_square, captured_piece, !white_to_move);
                move_mask = (1ULL << from) |  (1ULL << to);
                toggle_piece(moved_piece, white_to_move, move_mask);
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

        PieceType moved_piece = get_piece_at(to, white_to_move);

        if(flag >= PROMO_KNIGHT){
            moved_piece = PAWN;
            
            remove_piece(to, get_piece_at(to, white_to_move), white_to_move);
            place_piece(from, PAWN, white_to_move);
        }else{
            uint64_t move_mask = (1ULL << from) | (1ULL << to);
            toggle_piece(moved_piece, white_to_move, move_mask);
        }

        if(prev_state.captured_piece != EMPTY){
            place_piece(prev_state.captured_square, prev_state.captured_piece, prev_state.piece_was_white);
        }

        if (flag == KING_CASTLE) {
            if (white_to_move) {
            
                white_rooks ^= (1ULL << 5) | (1ULL << 7);
            } else {
                black_rooks ^= (1ULL << 61) | (1ULL << 63);
            }
        } else if (flag == QUEEN_CASTLE) {
            if (white_to_move) {
                white_rooks ^= (1ULL << 3) | (1ULL << 0);
            } else {
                black_rooks ^= (1ULL << 59) | (1ULL << 56);
            }
        }
    }

    void Board::generate_all_moves(MoveList &list){
        uint64_t enemy_pieces = white_to_move ? black_pieces() : white_pieces();
        uint64_t knights = white_to_move ? white_knights : black_knights;
        while(knights){
            int from = pop_lsb(knights);
            uint64_t attacks = MoveGen::pseudolegal_knight_moves(from, *this);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                list.push(encode_move(from, to, flag));
            }
        }

        uint64_t bishops = white_to_move ? white_bishops : black_bishops;
        while(bishops){
            int from = pop_lsb(bishops);
            uint64_t attacks = MoveGen::pseudolegal_bishop_moves(from, *this);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                list.push(encode_move(from, to, flag));
            }
        }

        uint64_t rooks = white_to_move ? white_rooks : black_rooks;
        while(rooks){
            int from = pop_lsb(rooks);
            uint64_t attacks = MoveGen::pseudolegal_rook_moves(from, *this);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                list.push(encode_move(from, to, flag));
            }
        }

        uint64_t queens = white_to_move ? white_queens : black_queens;
        while(queens){
            int from = pop_lsb(queens);
            uint64_t attacks = MoveGen::pseudolegal_queen_moves(from, *this);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                list.push(encode_move(from, to, flag));
            }
        }


        // Adding all the various pawn moves... so many...

        PawnMoves pawn_moves = MoveGen::pseudolegal_pawn_moves(*this);
        while(pawn_moves.single_push){
            int to = pop_lsb(pawn_moves.single_push);
            int from = white_to_move ? to - 8 : to + 8;
            uint64_t rank_mask = white_to_move ? MASK_8_RANK : MASK_1_RANK;

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
            int from = white_to_move ? to - 16 : to + 16;

            list.push(encode_move(from, to, DOUBLE_PAWN_PUSH));
        }

        while(pawn_moves.capture_left){
            int to = pop_lsb(pawn_moves.capture_left);
            int from = white_to_move ? to - 7 : to + 7;
            int flag = (1ULL << to) & en_passant_target ? EP_CAPTURE : CAPTURE;
            uint64_t rank_mask = white_to_move ? MASK_8_RANK : MASK_1_RANK;

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
            int from = white_to_move ? to - 9 : to + 9;
            int flag = ((1ULL << to) & en_passant_target) ? EP_CAPTURE : CAPTURE;

            uint64_t rank_mask = white_to_move ? MASK_8_RANK : MASK_1_RANK;

            if((1ULL << to) & rank_mask){
                list.push(encode_move(from, to, PROMO_CAPTURE_KNIGHT));
                list.push(encode_move(from, to, PROMO_CAPTURE_BISHOP));
                list.push(encode_move(from, to, PROMO_CAPTURE_ROOK));
                list.push(encode_move(from, to, PROMO_CAPTURE_QUEEN));
            }else{
                list.push(encode_move(from, to, flag));
            }
        }

        // Adding the king moves
        uint64_t king = white_to_move ? white_king : black_king;
        while(king){
            int from = pop_lsb(king);
            uint64_t attacks = MoveGen::pseudolegal_king_moves(from, *this);

            while(attacks){
                int to = pop_lsb(attacks);
                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;
                
                bool can_castle = true;

                if(white_to_move){
                    if(from == 4){
                        if(to == 2) {
                            flag = QUEEN_CASTLE;
                            if(is_square_attacked(4, false) || is_square_attacked(3, false)) can_castle = false;
                        }
                        if(to == 6) {
                            flag = KING_CASTLE;
                            if(is_square_attacked(4, false) || is_square_attacked(5, false)) can_castle = false;
                        }
                    }
                }else{
                    if(from == 60){
                        if(to == 58) {
                            flag = QUEEN_CASTLE;
                            if(is_square_attacked(60, true) || is_square_attacked(59, true)) can_castle = false;
                        }
                        if(to == 62) {
                            flag = KING_CASTLE;
                            if(is_square_attacked(60, true) || is_square_attacked(61, true)) can_castle = false;
                        }
                    }
                }

                if (can_castle) {
                    list.push(encode_move(from, to, flag));
                }
            }
        }

    }
