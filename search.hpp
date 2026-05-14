#include "board.hpp"
#include "movegen.hpp"
#include "evaluation.hpp"

class Searcher {
    public:
        Searcher();

        Move search(Board &board);
};
