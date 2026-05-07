#include <iostream>
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

int perft(Board &board, int depth){
    if (depth == 0) return 1;

    int nodes = 0;
    vector<Move> moves = board.generate_all_moves();

    for(Move m : moves){
        board.make_move(m);
        nodes += perft(board, depth - 1);
        board.unmake_move(m);
    }

    return nodes;
}

int main(){
    Board currentBoard;

    std::vector<Move> mosse;


    //currentBoard.print_board();

    for(int i = 0; i < 5; i++){
        cout << "perft " << i << ": " << perft(currentBoard, i) << endl;
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