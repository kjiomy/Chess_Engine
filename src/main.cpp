#include <iostream>
#include <chrono>
#include "board.hpp"
#include "uci.hpp"
#include "movegen.hpp"


// Moving the legality check in the perft to avoid wasting memory

using namespace std;
using namespace MoveGen;

int perft(Board &board, int depth){
    if (depth == 0) return 1;

    int nodes = 0;

    MoveList move_list;
    generate_all_moves(move_list, board);


    for(int i = 0; i < move_list.count; i++){
        Move m = move_list.moves[i];

        BoardState saved_state = board.make_move(m);

        bool moving_side = !board.white_to_move;
        uint64_t king = board.bitboards[moving_side][KING];

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

void perft_test(Board &currentBoard, int max_depth){
    cout << "--- Beginning peft test ---" << endl;
    //currentBoard.print_board();
    cout << "-------------------------" << endl;

    for(int depth = 1; depth <= max_depth; depth++){
        auto start_time = chrono::high_resolution_clock::now();

        int nodes = perft(currentBoard, depth);

        auto end_time = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsed = end_time - start_time;
        double seconds = elapsed.count();

        uint64_t nps = (seconds > 0.0) ? (nodes / seconds) : 0;


        cout << "Depth " << depth 
             << " | Nodes: " << nodes 
             << " | Time: " << seconds << " s" 
             << " | NPS: " << nps << endl;
    }

    cout << "--- TEST COMPLETATO ---" << endl;
}

int main(){
    Board currentBoard;

    perft_test(currentBoard, 6);


    //uci_loop(currentBoard);

    return 0;
}