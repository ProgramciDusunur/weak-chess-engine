#include "chess.hpp"
#include "bitboard.hpp"

using namespace chess;
using namespace std;

// Entire evaluation function is tuned with non other than Gedas' Texel tuner <3
// https://github.com/GediminasMasaitis/texel-tuner

// Helper: LSB index
int32_t lsb(uint64_t bb) {
    assert(bb != 0);
    return __builtin_ctzll(bb);
}

// Helper: Count no. set bits
int32_t count(uint64_t bb){
    return __builtin_popcountll(bb);
}

// Passed pawn masks
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

const uint64_t OUTER_2_SQ_RING_MASK[64] = {
    289363972639948800ull,    578729044791525376ull,
    1229798258109317120ull,    2459596516218634240ull,
    4919193032437268480ull,    9838386064874536960ull,
    1157688987024883712ull,    2315096499073056768ull,
    289360704169836544ull,    578721412634640384ull,
    1229782998090514432ull,    2459565996181028864ull,
    4919131992362057728ull,    9838263984724115456ull,
    1157443727212412928ull,    2314886354913198080ull,
    505533473516158976ull,    1083124541087023104ull,
    2238589255012057088ull,    4477178510024114176ull,
    8954357020048228352ull,    17908714040096456704ull,
    17298343833662128128ull,    16149943589319737344ull,
    1974740130922496ull,    4230955238621184ull,
    8744489277390848ull,    17488978554781696ull,
    34977957109563392ull,    69955914219126784ull,
    67571655600242688ull,    63085717145780224ull,
    7713828636416ull,    16527168900864ull,
    34158161239808ull,    68316322479616ull,
    136632644959232ull,    273265289918464ull,
    263951779688448ull,    246428582600704ull,
    30132143111ull,    64559253519ull,
    133430317343ull,    266860634686ull,
    533721269372ull,    1067442538744ull,
    1031061639408ull,    962611650784ull,
    117703684ull,    252184584ull,
    521212177ull,    1042424354ull,
    2084848708ull,    4169697416ull,
    4027584528ull,    3760201760ull,
    459780ull,    985096ull,
    2035985ull,    4071970ull,
    8143940ull,    16287880ull,
    15732752ull,    14688288ull
};

// Check if square is passed pawn for white
bool is_white_passed_pawn(int32_t square, uint64_t black_pawns) {
    return (WHITE_PASSED_MASK[square] & black_pawns) == 0;
}

// Check if square is passed pawn for black
bool is_black_passed_pawn(int32_t square, uint64_t white_pawns) {
    return (BLACK_PASSED_MASK[square] & white_pawns) == 0;
}

// Count passed pawns for white
int32_t count_white_passed_pawns(uint64_t white_pawns, uint64_t black_pawns) {
    int32_t count = 0;
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
int32_t count_black_passed_pawns(uint64_t black_pawns, uint64_t white_pawns) {
    int count = 0;
    uint64_t bb = black_pawns;
    while (bb) {
        int32_t sq = __builtin_ctzll(bb); // or lsb(bb)
        if (is_black_passed_pawn(sq, white_pawns))
            ++count;
        bb &= bb - 1;
    }
    return count;
}
