#include "board.hpp"
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

// Functions to get various pieces/squares

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

// Ausiliary functions 

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



// Functions to calculate the pseudo-legal moves for every piece

    uint64_t Board::pseudolegal_knight_moves(uint8_t square){
        uint64_t k = 1ULL << square;
        uint64_t friendly_pieces = white_to_move ? white_pieces() : black_pieces();

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

    uint64_t Board::pseudolegal_rook_moves(uint8_t square){
        uint64_t attacks = 0;
        uint64_t empty = empty_squares();
        uint64_t enemy = white_to_move ? black_pieces() : white_pieces();
        uint64_t friendly = white_to_move ? white_pieces() : black_pieces();

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

    uint64_t Board::pseudolegal_bishop_moves(uint8_t square){
        uint64_t attacks = 0;
        uint64_t enemy = white_to_move ? black_pieces() : white_pieces();
        uint64_t friendly = white_to_move ? white_pieces() : black_pieces();

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

    uint64_t Board::pseudolegal_queen_moves(uint8_t square){
        return pseudolegal_rook_moves(square) | pseudolegal_bishop_moves(square);
    }

    uint64_t Board::pseudolegal_king_moves(uint8_t square){
        uint64_t k = 1ULL << square;
        uint64_t friendly_pieces = white_to_move ? white_pieces() : black_pieces();
        uint64_t occupied_squares = all_pieces();

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
        if(white_to_move){
            // White side
            if(square == 4){
                // King side
                if((castling_rights & WK) && !(occupied_squares & (1ULL  << 5 | 1ULL << 6))){
                    // Da aggiungere se lo square è attaccato!!!!
                    attacks |= (1ULL << 6);
                }

                // Queen side
                if((castling_rights & WQ) && !(occupied_squares & (1ULL  << 3 | 1ULL << 2 | 1ULL << 1))){
                    // Da aggiungere se lo square è attaccato!!!!!
                    attacks |= (1ULL << 2);
                }
            }
        }else {
            // Black side
            if(square == 60){
                // King side
                if((castling_rights & BK) && !(occupied_squares &(1ULL << 61 | 1ULL << 62))){
                    // Da aggiungere Square attaccato!!!
                    attacks |= (1ULL << 62);
                }

                if((castling_rights & BQ) && !(occupied_squares &(1ULL << 59 | 1ULL << 58 | 1ULL << 57))){
                    //logica attaccato!!!!
                    attacks |= (1ULL << 58);
                }
            }
        }


        return attacks;
    }

    PawnMoves Board::pseudolegal_pawn_moves(){
        PawnMoves moves;
        
        if(white_to_move){
            uint64_t empty = empty_squares();
            uint64_t enemy = black_pieces() | en_passant_target;


            moves.single_push = (white_pawns << 8) & empty;
            moves.double_push = ((moves.single_push & 0x0000000000FF0000) << 8) & empty;

            moves.capture_left = (white_pawns << 7) & NOT_H_FILE & enemy;
            moves.capture_right = (white_pawns << 9) & NOT_A_FILE & enemy;
        return moves;
        }else {
            uint64_t empty = empty_squares();
            uint64_t enemy = white_pieces() | en_passant_target;


            moves.single_push = (black_pawns >> 8) & empty;
            moves.double_push = ((moves.single_push & 0x0000FF0000000000) >> 8) & empty;

            moves.capture_left = (black_pawns >> 7) & NOT_A_FILE & enemy;
            moves.capture_right = (black_pawns >> 9) & NOT_H_FILE & enemy;
            return moves;
        }
    }



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
   
   
    //da finire con tutti i vari flag ecc...
    void Board::make_move(Move move){
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
        history.push_back(current_state);

        en_passant_target = 0;

        uint64_t move_mask = (1ULL << from) |  (1ULL << to);

        toggle_piece(moved_piece, white_to_move, move_mask);

        if(captured_piece != EMPTY && flags != EP_CAPTURE){
            remove_piece(to, captured_piece, !white_to_move);
        }

        if(captured_piece != EMPTY && flags == EP_CAPTURE){
            //implementa la logica...
        }

        //devo implementare TUTTI i flags...
        if(flags == DOUBLE_PAWN_PUSH){
            en_passant_target = white_to_move ? (1ULL << (from + 8)) : (1ULL << (from - 8));
        }else if(flags == KING_CASTLE){

        }

        //aggiornare i diritti di arrocco!!!;
        white_to_move = !white_to_move;
    }

    void Board::unmake_move(Move move){
        BoardState prev_state = history.back();
        history.pop_back();

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

    std::vector<Move> Board::generate_all_moves(){
        std::vector<Move> moves;
        moves.reserve(256);

        uint64_t enemy_pieces = white_to_move ? black_pieces() : white_pieces();
        uint64_t knights = white_to_move ? white_knights : black_knights;
        while(knights){
            int from = pop_lsb(knights);
            uint64_t attacks = pseudolegal_knight_moves(from);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                moves.push_back(encode_move(from, to, flag));
            }
        }

        uint64_t bishops = white_to_move ? white_bishops : black_bishops;
        while(bishops){
            int from = pop_lsb(bishops);
            uint64_t attacks = pseudolegal_bishop_moves(from);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                moves.push_back(encode_move(from, to, flag));
            }
        }

        uint64_t rooks = white_to_move ? white_rooks : black_rooks;
        while(rooks){
            int from = pop_lsb(rooks);
            uint64_t attacks = pseudolegal_rook_moves(from);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                moves.push_back(encode_move(from, to, flag));
            }
        }

        uint64_t queens = white_to_move ? white_queens : black_queens;
        while(queens){
            int from = pop_lsb(queens);
            uint64_t attacks = pseudolegal_queen_moves(from);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                moves.push_back(encode_move(from, to, flag));
            }
        }

        PawnMoves pawn_moves = pseudolegal_pawn_moves();
        while(pawn_moves.single_push){
                int to = pop_lsb(pawn_moves.single_push);
                int from = white_to_move ? to - 8 : to + 8;

                moves.push_back(encode_move(from, to, QUIET_MOVE));
        }

        while(pawn_moves.double_push){
                int to = pop_lsb(pawn_moves.double_push);
                int from = white_to_move ? to - 16 : to + 16;

                moves.push_back(encode_move(from, to, QUIET_MOVE));
        }

        
        return moves;
    }
