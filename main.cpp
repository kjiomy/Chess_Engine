#include <iostream>
#include "board.cpp"

using namespace std;

int main(){
    Board* currentBoard = new Board();

    cout << "--Pezzi Bianchi--" << endl;
    currentBoard->print_bitboard(currentBoard->white_rooks);
    currentBoard->print_bitboard(currentBoard->white_knights);


}