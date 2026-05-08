#include <iostream>
#include <chrono>
#include "board.hpp"
#include "uci.hpp"

// Source - https://stackoverflow.com/a/16421677
// Posted by Christopher Smith, modified by community. See post 'Timeline' for change history
// Retrieved 2026-05-05, License - CC BY-SA 3.0

#include  <random>
#include  <iterator>

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}


using namespace std;

// Moving the legality check in the perft to avoid wasting memory

int perft(Board &board, int depth){
    if (depth == 0) return 1;

    int nodes = 0;

    MoveList move_list;
    board.generate_all_moves(move_list);


    for(int i = 0; i < move_list.count; i++){
        Move m = move_list.moves[i];

        board.make_move(m);

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

        board.unmake_move(m);
    }

    return nodes;
}

int main(){
    Board currentBoard;

    std::vector<Move> mosse;

    currentBoard.print_board();
    cout << "---------------------------------" << endl;

    int max_depth = 7;

    for(int i = 0; i < max_depth; i++){
        auto start_time = chrono::high_resolution_clock::now();
        uint64_t nodes = perft(currentBoard, i);
        auto end_time = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsed = end_time - start_time;
        double seconds = elapsed.count();

        uint64_t nps = (seconds > 0.0) ? (nodes / seconds) : 0;

        cout << "Depth " << max_depth << " | Nodes: " << nodes << " | Time: " << seconds << " s" << " | NPS: " << nps << endl;
    }

    

    
    /*
    for(int i = 0; i < 20; i++){
        mosse = currentBoard.generate_all_moves();

        Move mossa = *select_randomly(mosse.begin(), mosse.end());

        cout << endl << get_move_from(mossa) << " : " << get_move_to(mossa) << ", " << get_move_flags(mossa) << endl << endl;

        currentBoard.make_move(mossa);

        currentBoard.print_board();
    }
    */
    
    

    //uci_loop(currentBoard);

}