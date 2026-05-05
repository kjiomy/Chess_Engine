// Defining some filter to calculate legal moves
constexpr uint64_t NOT_A_FILE  = 0xFEFEFEFEFEFEFEFE;
constexpr uint64_t NOT_H_FILE = 0x7F7F7F7F7F7F7F7F;
constexpr uint64_t NOT_AB_FILE = 0xFCFCFCFCFCFCFCFC;
constexpr uint64_t NOT_GH_FILE = 0x3F3F3F3F3F3F3F3F;
constexpr uint64_t MASK_64 = 0xFFFFFFFFFFFFFFFF;

typedef uint16_t Move;



enum Castling  {
    WK = 1,
    WQ = 2,
    BK = 4,
    BQ = 8
};

enum MoveFlag {
        QUIET_MOVE = 0,
        DOUBLE_PAWN_PUSH = 1,
        KING_CASTLE = 2,
        QUEEN_CASTLE = 3,
        CAPTURE = 4,
        EP_CAPTURE = 5,

        PROMO_KNIGHT = 8,
        PROMO_BISHOP = 9,
        PROMO_ROOK = 10,
        PROMO_QUEEN = 11,

        PROMO_CAPTURE_KNIGHT = 12,
        PROMO_CAPTURE_BISHOP = 13,
        PROMO_CAPTURE_ROOK = 14,
        PROMO_CAPTURE_QUEEN = 15
    };

enum PieceType { EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

struct BoardState{
    uint8_t castling_rights;
    uint64_t en_passant_target;
    int captured_pieces;
};

struct PawnMoves{
    uint64_t single_push;
    uint64_t double_push;
    uint64_t capture_left;
    uint64_t capture_right;
    uint64_t get_all(){
        return single_push | double_push | capture_left | capture_right;
    }
};



inline Move encode_move(int from, int to, int flags){
    return (flags << 12) | (to << 6) | from;
}

inline int get_move_flags(Move move){
    return (move >> 12) & 0x0F;
}

inline int get_move_to(Move move){
    return (move >> 6) & 0x3F;
}

inline int get_move_from(Move move){
    return move & 0x3F;
}

inline int pop_lsb(uint64_t& bb){
    int lsb = __builtin_ctzll(bb);

    bb &= bb -1;

    return lsb;
}