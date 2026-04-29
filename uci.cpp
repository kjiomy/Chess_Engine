#include "uci.hpp"

using namespace std;

void uci_loop(Board& currentBoard){
    string line;

    while(getline(cin, line)){
        if(line.empty()) continue;

        istringstream iss(line);
        string command;
        iss >> command;

        if(command == "uci"){
            cout << "Id name ChessTest" << endl;
            cout << "Id author Kim" << endl;
            cout << "uciok" << endl;
        }

        else if(command == "isready"){
            cout << "readyok" << endl;  
        }
        else if(command == "ucinewgame"){
            currentBoard.init_board();
        }
        else if(command == "position"){
            cout << "info String ricevuta posizione" << endl;
        }
        else if(command == "go"){
            cout << "info string Thinking..." << endl;

            cout << "bestmove e2e4" << endl;
        }
        else if(command == "quit"){
            break;
        }

        else{
            cout << "boh, che hai scritto?" << endl;
        }
    }
}   