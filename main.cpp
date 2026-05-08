#include <iostream>
#include <chrono>
#include "board.hpp"
#include "uci.hpp"


// Moving the legality check in the perft to avoid wasting memory

using namespace std;

int perft(Board &board, int depth){
    if (depth == 0) return 1;

    int nodes = 0;

    MoveList move_list;
    board.generate_all_moves(move_list);


    for(int i = 0; i < move_list.count; i++){
        Move m = move_list.moves[i];

        BoardState saved_state = board.make_move(m);

        bool moving_side = !board.white_to_move;
        uint64_t king = moving_side ? board.white_king : board.black_king;

        if(king != 0){
            uint8_t king_square = __builtin_ctzll(king);

            if(!board.is_square_attacked(king_square, board.white_to_move)){
                if(depth == 1){
                    nodes++;
                }else{
                    nodes += perft(board, depth - 1);   
                }
                
            }
        }   

        board.unmake_move(m, saved_state);
    }

    return nodes;
}

int main(){
    Board currentBoard;


    uci_loop(currentBoard);
}