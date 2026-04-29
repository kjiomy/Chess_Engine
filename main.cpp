#include <iostream>
#include "board.hpp"
#include "uci.hpp"

using namespace std;

int main(){
    Board currentBoard;

    std::vector<Move> mosse;

    mosse = currentBoard.generate_all_moves();

    for(auto mossa : mosse){
        cout << "from: " << get_move_from(mossa) << ", to " << get_move_to(mossa) << ", flags: " << get_move_flags(mossa) << endl;
    }

    //uci_loop(currentBoard);

}