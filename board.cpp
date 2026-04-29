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

        castling_rights = 0x0F;
        en_passant_target = 0x0000000000000000;
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

    uint64_t Board::knight_moves(uint8_t square){
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

    uint64_t Board::rook_moves(uint8_t square){
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

    uint64_t Board::bishop_moves(uint8_t square){
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

    uint64_t Board::queen_moves(uint8_t square){
        return rook_moves(square) | bishop_moves(square);
    }

    uint64_t Board::king_moves(uint8_t square){
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

    PawnMoves Board::pawn_moves(){
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

    void Board::make_move(Move move){
        int flags = get_move_flags(move);
        int from = get_move_from(move);
        int to = get_move_to(move);

        PieceType moved_piece = get_piece_at(from, white_to_move);
        PieceType captured_piece = get_piece_at(to, !white_to_move);

        BoardState current_state = {castling_rights, en_passant_target, captured_piece};
        history.push_back(current_state);

        en_passant_target = 0;

        uint64_t move_mask = (1ULL << from) |  (1ULL << to);

        if(white_to_move){
            switch(moved_piece){
                case PAWN: white_pawns ^= move_mask; break;
                case KNIGHT: white_knights ^= move_mask; break;
                case BISHOP: white_bishops ^= move_mask; break;
                case ROOK: white_rooks ^= move_mask; break;
                case QUEEN: white_queens ^= move_mask; break;
                case KING: white_king ^= move_mask; break;
                case EMPTY: break;
            }
        }else{
            switch(moved_piece){
                case PAWN: black_pawns ^= move_mask; break;
                case KNIGHT: black_knights ^= move_mask; break;
                case BISHOP: black_bishops ^= move_mask; break;
                case ROOK: black_rooks ^= move_mask; break;
                case QUEEN: black_queens ^= move_mask; break;
                case KING: black_king ^= move_mask; break;
                case EMPTY: break;
            }
        }

        if(captured_piece != EMPTY && flags != EP_CAPTURE){
            uint64_t capture_mask = ~(1ULL << to);

            if(white_to_move){
                switch(captured_piece){
                    case PAWN: black_pawns &= capture_mask; break;
                    case KNIGHT: black_knights &= capture_mask; break;
                    case BISHOP: black_bishops &= capture_mask; break;
                    case ROOK: black_rooks &= capture_mask; break;
                    case QUEEN: black_queens &= capture_mask; break;
                    case KING: black_king &= capture_mask; break;
                    case EMPTY: break;
                }
            }else{
                switch(captured_piece){
                    case PAWN: white_pawns &= capture_mask; break;
                    case KNIGHT: white_knights &= capture_mask; break;
                    case BISHOP: white_bishops &= capture_mask; break;
                    case ROOK: white_rooks &= capture_mask; break;
                    case QUEEN: white_queens &= capture_mask; break;
                    case KING: white_king &= capture_mask; break;
                    case EMPTY: break;
                }
            }
        }

        //devo implementare TUTTI i flags...
        if(flags == DOUBLE_PAWN_PUSH){
            en_passant_target = white_to_move ? (1ULL << (from + 8)) : (1ULL << (from - 8));
        }else if(flags == KING_CASTLE){

        }

        //aggiornare i diritti di arrocco!!!;
        white_to_move = !white_to_move;
    }

    //da implementare unmake move!!!!!

    std::vector<Move> Board::generate_all_moves(){
        std::vector<Move> moves;
        moves.reserve(256);

        uint64_t enemy_pieces = white_to_move ? black_pieces() : white_pieces();
        uint64_t knights = white_to_move ? white_knights : black_knights;
        while(knights){
            int from = pop_lsb(knights);
            uint64_t attacks = knight_moves(from);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                moves.push_back(encode_move(from, to, flag));
            }
        }

        uint64_t bishops = white_to_move ? white_bishops : black_bishops;
        while(bishops){
            int from = pop_lsb(bishops);
            uint64_t attacks = bishop_moves(from);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                moves.push_back(encode_move(from, to, flag));
            }
        }

        uint64_t rooks = white_to_move ? white_rooks : black_rooks;
        while(rooks){
            int from = pop_lsb(rooks);
            uint64_t attacks = rook_moves(from);

            while(attacks){
                int to = pop_lsb(attacks);

                int flag = ((1ULL << to) & enemy_pieces) ? CAPTURE : QUIET_MOVE;

                moves.push_back(encode_move(from, to, flag));
            }
        }

        
        return moves;
    }
