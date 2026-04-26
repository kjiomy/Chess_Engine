#include <iostream>
#include "board.hpp"

using namespace std;

int main(){
    Board currentBoard;

    cout << "--Pezzi Bianchi--" << endl;

    currentBoard.print_bitboard(currentBoard.queen_attacks(36));
}