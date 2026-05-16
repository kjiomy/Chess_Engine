#include "uci.hpp"
#include "search.hpp"
#include "movegen.hpp"

using namespace std;

string move_to_string(Move move){
    int from = get_move_from(move);
    int to = get_move_to(move);
    int flag = get_move_flags(move);

    string s = "";
    s += (char)('a' + (from % 8));
    s += (char)('1' + (from / 8));
    s += (char)('a' + (to % 8));
    s += (char)('1' + (to / 8));

    // Adding the flags for promotion

    if (flag == PROMO_QUEEN || flag == PROMO_CAPTURE_QUEEN) s += "q";
    else if (flag == PROMO_ROOK || flag == PROMO_CAPTURE_ROOK) s += "r";
    else if (flag == PROMO_BISHOP || flag == PROMO_CAPTURE_BISHOP) s += "b";
    else if (flag == PROMO_KNIGHT || flag == PROMO_CAPTURE_KNIGHT) s += "n";

    return s;
}

Move parse_move(Board &board, string s){
    MoveList list;
    MoveGen::generate_all_moves(list, board);

    for (int i = 0; i < list.count; i++) {
        Move m = list.moves[i];
        
        // Cheking if move is legal
        BoardState state = board.make_move(m);
        bool moving_side = !board.white_to_move;
        uint64_t king = board.bitboards[moving_side][KING];
        
        bool legal = true;
        if (king != 0) {
            uint8_t king_sq = __builtin_ctzll(king);
            if (board.is_square_attacked(king_sq, board.white_to_move)) legal = false;
        } else {
            legal = false;
        }
        board.unmake_move(m, state);

        // if it's legal and string is equal we found the move
        if (legal && move_to_string(m) == s) {
            return m;
        }
    }
    return 0; 
}

void uci_loop(Board& currentBoard){
    string line;

    while(getline(cin, line)){
        if(line.empty()) continue;

        istringstream iss(line);
        string command;
        iss >> command;

        if(command == "uci"){
            cout << "id name KimEngine v1.0" << endl;
            cout << "id author Kim" << endl;
            cout << "uciok" << endl;
        }
        else if(command == "isready"){
            cout << "readyok" << endl;  
        }
        else if(command == "ucinewgame"){
            currentBoard.init_board();
        }
        else if(command == "position"){
            string token;
            iss >> token;

            // there is a starting position
            if(token == "startpos"){
                currentBoard.init_board();
                iss >> token; 
            }

            
            if(token == "moves"){
                string move_str;
                while(iss >> move_str){
                    Move m = parse_move(currentBoard, move_str);
                    if(m != 0){
                        currentBoard.make_move(m); 
                    }
                }
            }
        }
        else if(command == "go"){
            Move best_move = Search::get_greedy_move(currentBoard);

            if(best_move != 0){
                cout << "bestmove " << move_to_string(best_move) << endl;
            } else {
                cout << "bestmove 0000" << endl; 
            }
        }

        
        else if(command == "quit"){
            break;
        }
    }
}