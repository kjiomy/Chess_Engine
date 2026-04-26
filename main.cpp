#include <iostream>
#include "board.hpp"

using namespace std;

int main(){
    Board currentBoard;

    cout << "--Pezzi Bianchi--" << endl;
    currentBoard.print_bitboard(currentBoard.knight_attacks(26, currentBoard.white_pieces()));

    currentBoard.print_bitboard(currentBoard.black_pawn_moves().get_all());



}