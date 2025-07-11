#include <iostream>
#include <cstdint>
#include <cassert>

using namespace std;

enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

const uint64_t WHITE_PASSED_MASK[64] = {
    217020518514230016ull,    506381209866536704ull,
    1012762419733073408ull,    2025524839466146816ull,
    4051049678932293632ull,    8102099357864587264ull,
    16204198715729174528ull,    13889313184910721024ull,
    217020518514229248ull,    506381209866534912ull,
    1012762419733069824ull,    2025524839466139648ull,
    4051049678932279296ull,    8102099357864558592ull,
    16204198715729117184ull,    13889313184910671872ull,
    217020518514032640ull,    506381209866076160ull,
    1012762419732152320ull,    2025524839464304640ull,
    4051049678928609280ull,    8102099357857218560ull,
    16204198715714437120ull,    13889313184898088960ull,
    217020518463700992ull,    506381209748635648ull,
    1012762419497271296ull,    2025524838994542592ull,
    4051049677989085184ull,    8102099355978170368ull,
    16204198711956340736ull,    13889313181676863488ull,
    217020505578799104ull,    506381179683864576ull,
    1012762359367729152ull,    2025524718735458304ull,
    4051049437470916608ull,    8102098874941833216ull,
    16204197749883666432ull,    13889312357043142656ull,
    217017207043915776ull,    506373483102470144ull,
    1012746966204940288ull,    2025493932409880576ull,
    4050987864819761152ull,    8101975729639522304ull,
    16203951459279044608ull,    13889101250810609664ull,
    216172782113783808ull,    504403158265495552ull,
    1008806316530991104ull,    2017612633061982208ull,
    4035225266123964416ull,    8070450532247928832ull,
    16140901064495857664ull,    13835058055282163712ull,
    0ull,    0ull,
    0ull,    0ull,
    0ull,    0ull,
    0ull,    0ull
};

const uint64_t BLACK_PASSED_MASK[64] = {
    0ull,    0ull,
    0ull,    0ull,
    0ull,    0ull,
    0ull,    0ull,
    3ull,    7ull,
    14ull,    28ull,
    56ull,    112ull,
    224ull,    192ull,
    771ull,    1799ull,
    3598ull,    7196ull,
    14392ull,    28784ull,
    57568ull,    49344ull,
    197379ull,    460551ull,
    921102ull,    1842204ull,
    3684408ull,    7368816ull,
    14737632ull,    12632256ull,
    50529027ull,    117901063ull,
    235802126ull,    471604252ull,
    943208504ull,    1886417008ull,
    3772834016ull,    3233857728ull,
    12935430915ull,    30182672135ull,
    60365344270ull,    120730688540ull,
    241461377080ull,    482922754160ull,
    965845508320ull,    827867578560ull,
    3311470314243ull,    7726764066567ull,
    15453528133134ull,    30907056266268ull,
    61814112532536ull,    123628225065072ull,
    247256450130144ull,    211934100111552ull,
    847736400446211ull,    1978051601041159ull,
    3956103202082318ull,    7912206404164636ull,
    15824412808329272ull,    31648825616658544ull,
    63297651233317088ull,    54255129628557504ull
};

// Helper: Set a bit
inline void set_bit(uint64_t& bb, int square) {
    bb |= (1ULL << square);
}

// Helper: LSB index
inline int lsb(uint64_t bb) {
    assert(bb != 0);
    return __builtin_ctzll(bb);
}

// Print bitboard in top-down format
void print_bitboard(uint64_t bb) {
    int board[64]{};
    while (bb) {
        int sq = lsb(bb);
        board[sq ^ 56] = 1; // Flip for top-down view
        bb &= bb - 1;
    }

    for (int i = 0; i < 64; ++i) {
        if (i % 8 == 0 && i != 0) cout << "\n";
        cout << board[i] << " ";
    }
    cout << "\n\n";
}

// Generate white passed pawn mask for a square
uint64_t white_passed_mask(int square) {
    int file = square % 8;
    int rank = square / 8;
    uint64_t mask = 0;

    for (int df = -1; df <= 1; ++df) {
        int f = file + df;
        if (f < 0 || f > 7) continue;
        for (int r = rank + 1; r < 8; ++r) {
            int sq = r * 8 + f;
            mask |= (1ULL << sq);
        }
    }
    return mask;
}

// Generate black passed pawn mask for a square
uint64_t black_passed_mask(int square) {
    int file = square % 8;
    int rank = square / 8;
    uint64_t mask = 0;

    for (int df = -1; df <= 1; ++df) {
        int f = file + df;
        if (f < 0 || f > 7) continue;
        for (int r = rank - 1; r >= 0; --r) {
            int sq = r * 8 + f;
            mask |= (1ULL << sq);
        }
    }
    return mask;
}

// Check if square is passed pawn for white
bool is_white_passed_pawn(int square, uint64_t black_pawns) {
    return (WHITE_PASSED_MASK[square] & black_pawns) == 0;
}

// Check if square is passed pawn for black
bool is_black_passed_pawn(int square, uint64_t white_pawns) {
    return (BLACK_PASSED_MASK[square] & white_pawns) == 0;
}

// Get passed pawns for white
uint64_t get_white_passed_pawns(uint64_t white_pawns, uint64_t black_pawns) {
    uint64_t result = 0;
    uint64_t bb = white_pawns;
    while (bb) {
        int sq = lsb(bb);
        if (is_white_passed_pawn(sq, black_pawns))
            result |= (1ULL << sq);
        bb &= bb - 1;
    }
    return result;
}

// Get passed pawns for black
uint64_t get_black_passed_pawns(uint64_t black_pawns, uint64_t white_pawns) {
    uint64_t result = 0;
    uint64_t bb = black_pawns;
    while (bb) {
        int sq = lsb(bb);
        if (is_black_passed_pawn(sq, white_pawns))
            result |= (1ULL << sq);
        bb &= bb - 1;
    }
    return result;
}

// Count passed pawns for white
int count_white_passed_pawns(uint64_t white_pawns, uint64_t black_pawns) {
    int count = 0;
    uint64_t bb = white_pawns;
    while (bb) {
        int sq = __builtin_ctzll(bb); // or lsb(bb)
        if (is_white_passed_pawn(sq, black_pawns))
            ++count;
        bb &= bb - 1; // Clear least significant bit
    }
    return count;
}

// Count passed pawns for black
int count_black_passed_pawns(uint64_t black_pawns, uint64_t white_pawns) {
    int count = 0;
    uint64_t bb = black_pawns;
    while (bb) {
        int sq = __builtin_ctzll(bb); // or lsb(bb)
        if (is_black_passed_pawn(sq, white_pawns))
            ++count;
        bb &= bb - 1;
    }
    return count;
}


// Test
int main() {
    uint64_t white_pawns = 0;
    uint64_t black_pawns = 0;

    set_bit(white_pawns, A1); 
    set_bit(white_pawns, A2); 
    set_bit(white_pawns, B1); 
    set_bit(white_pawns, C1);
    set_bit(white_pawns, D1);

    set_bit(black_pawns, A8);
    set_bit(black_pawns, C8);
    set_bit(black_pawns, F8); 

    cout << "White pawns:\n";
    print_bitboard(white_pawns);

    cout << "Black pawns:\n";
    print_bitboard(black_pawns);

    uint64_t white_passed = get_white_passed_pawns(white_pawns, black_pawns);
    uint64_t black_passed = get_black_passed_pawns(black_pawns, white_pawns);

    cout << "White passed pawns:\n";
    print_bitboard(white_passed);
    cout << "No. white passed pawns\n";
    cout << count_white_passed_pawns(white_pawns, black_pawns) << "\n";

    cout << "Black passed pawns:\n";
    print_bitboard(black_passed);
    cout << count_black_passed_pawns(black_pawns, white_pawns) << "\n";
}
