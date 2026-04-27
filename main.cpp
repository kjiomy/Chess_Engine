#include <iostream>
#include "board.hpp"

using namespace std;

int main(){
    Board currentBoard;

    cout << "--Pezzi Bianchi--" << endl;

    currentBoard.print_bitboard(currentBoard.pawn_moves().get_all());

    cout << currentBoard.get_piece_at(4, currentBoard.white_to_move) << endl;
}