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

int main(){
    Board currentBoard;

    std::vector<Move> mosse;

    currentBoard.print_board();


    for(int i = 0; i < 20; i++){
        mosse = currentBoard.generate_all_moves();

        Move mossa = *select_randomly(mosse.begin(), mosse.end());

        currentBoard.make_move(mossa);

        currentBoard.print_board();
    }

    
    

    //uci_loop(currentBoard);

}