#include <stdint.h>

#include "chess.hpp"
#include "packing.hpp"
#include "eval.hpp"
#include "defaults.hpp"

using namespace chess;
using namespace std;

// Entire evaluation function is tuned with non other than Gedas' Texel tuner <3
// https://github.com/GediminasMasaitis/texel-tuner

// Helper: LSB index
inline int lsb(uint64_t bb) {
    assert(bb != 0);
    return __builtin_ctzll(bb);
}

// Helper: Count no. set bits
inline int count(uint64_t bb){
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
inline bool is_white_passed_pawn(int32_t square, uint64_t black_pawns) {
    return (WHITE_PASSED_MASK[square] & black_pawns) == 0;
}

// Check if square is passed pawn for black
inline bool is_black_passed_pawn(int32_t square, uint64_t white_pawns) {
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


// Piece Square Tables (which includes the material value of each piece)
// Each piece type has its own piece square table, whiched it used
// to evaluate its positioning on the board
const int32_t PSQT[6][64] = {
    {
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
        S(130, 264), S(160, 255), S(139, 256), S(162, 209), S(154, 206), S(137, 219), S(73, 261), S(36, 273),
        S(62, 204), S(79, 213), S(107, 180), S(111, 158), S(120, 149), S(138, 142), S(123, 184), S(75, 179),
        S(51, 136), S(75, 127), S(74, 110), S(81, 98), S(96, 94), S(89, 99), S(92, 116), S(70, 114),
        S(43, 112), S(68, 112), S(69, 95), S(82, 91), S(85, 90), S(78, 92), S(79, 104), S(59, 95),
        S(44, 105), S(64, 110), S(65, 93), S(71, 100), S(80, 97), S(73, 93), S(92, 100), S(67, 92),
        S(46, 109), S(67, 113), S(59, 99), S(61, 109), S(73, 111), S(78, 100), S(97, 100), S(60, 93),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    },

    {
        S(17, 82), S(18, 141), S(56, 123), S(88, 116), S(122, 119), S(73, 94), S(50, 144), S(64, 58),
        S(115, 138), S(113, 123), S(101, 116), S(110, 118), S(91, 112), S(155, 98), S(120, 119), S(149, 122),
        S(111, 114), S(110, 114), S(154, 164), S(155, 169), S(186, 159), S(194, 150), S(122, 111), S(127, 108),
        S(113, 124), S(88, 130), S(138, 174), S(161, 175), S(128, 183), S(156, 177), S(82, 136), S(136, 120),
        S(101, 125), S(80, 119), S(126, 172), S(125, 173), S(137, 175), S(128, 165), S(94, 121), S(110, 116),
        S(84, 111), S(70, 112), S(113, 151), S(122, 163), S(132, 160), S(119, 144), S(86, 106), S(100, 110),
        S(97, 136), S(86, 114), S(63, 108), S(81, 108), S(79, 107), S(73, 104), S(93, 104), S(120, 144),
        S(74, 127), S(113, 127), S(76, 108), S(89, 111), S(93, 110), S(92, 102), S(111, 135), S(104, 113)
    },

    {
        S(108, 164), S(82, 172), S(85, 167), S(48, 177), S(63, 172), S(80, 163), S(100, 161), S(84, 162),
        S(111, 154), S(128, 154), S(118, 156), S(105, 158), S(120, 149), S(119, 152), S(118, 161), S(110, 154),
        S(123, 171), S(139, 158), S(135, 156), S(139, 148), S(129, 154), S(157, 157), S(141, 159), S(131, 172),
        S(120, 166), S(125, 163), S(129, 156), S(139, 167), S(136, 159), S(130, 161), S(126, 158), S(110, 167),
        S(121, 159), S(112, 162), S(115, 162), S(136, 158), S(131, 158), S(122, 156), S(117, 159), S(131, 152),
        S(125, 162), S(125, 160), S(126, 157), S(124, 155), S(126, 158), S(128, 156), S(130, 152), S(142, 154),
        S(135, 166), S(134, 150), S(134, 142), S(121, 154), S(128, 152), S(131, 147), S(143, 157), S(139, 149),
        S(129, 155), S(138, 162), S(131, 152), S(117, 157), S(124, 156), S(118, 164), S(132, 150), S(144, 140)
    },

    {
        S(184, 295), S(166, 303), S(163, 314), S(160, 311), S(171, 306), S(193, 299), S(192, 297), S(207, 292),
        S(166, 298), S(159, 312), S(174, 316), S(188, 309), S(167, 314), S(195, 301), S(188, 296), S(215, 281),
        S(156, 297), S(177, 298), S(173, 300), S(175, 299), S(199, 290), S(189, 288), S(228, 279), S(199, 276),
        S(151, 299), S(160, 295), S(159, 304), S(167, 299), S(161, 291), S(166, 284), S(167, 283), S(171, 279),
        S(137, 292), S(135, 295), S(143, 296), S(154, 294), S(148, 293), S(134, 290), S(149, 282), S(144, 279),
        S(131, 290), S(135, 288), S(142, 287), S(144, 289), S(144, 287), S(141, 279), S(161, 264), S(149, 268),
        S(132, 285), S(141, 285), S(154, 284), S(155, 285), S(155, 279), S(145, 276), S(158, 269), S(141, 275),
        S(148, 288), S(150, 286), S(158, 291), S(160, 287), S(159, 282), S(142, 285), S(156, 279), S(146, 279)
    },

    {
        S(332, 544), S(318, 560), S(333, 578), S(361, 559), S(348, 567), S(347, 571), S(396, 516), S(358, 542),
        S(346, 536), S(324, 563), S(321, 593), S(312, 606), S(301, 628), S(343, 583), S(344, 567), S(385, 557),
        S(352, 543), S(345, 551), S(343, 580), S(341, 586), S(348, 590), S(368, 573), S(381, 545), S(371, 547),
        S(335, 556), S(340, 563), S(332, 573), S(329, 588), S(328, 590), S(338, 579), S(347, 576), S(345, 565),
        S(343, 549), S(331, 566), S(332, 565), S(338, 575), S(333, 573), S(334, 562), S(340, 557), S(348, 553),
        S(338, 536), S(344, 542), S(339, 553), S(334, 548), S(337, 553), S(340, 545), S(350, 529), S(348, 535),
        S(347, 531), S(346, 529), S(352, 524), S(349, 533), S(349, 534), S(347, 501), S(355, 480), S(379, 467),
        S(343, 530), S(341, 526), S(345, 526), S(351, 534), S(348, 514), S(323, 515), S(345, 500), S(352, 500)
    },

    {
        S(34, -96), S(22, -43), S(71, -36), S(-62, 12), S(-20, -4), S(-8, 0), S(20, -10), S(118, -114),
        S(-91, 0), S(11, 27), S(-18, 40), S(92, 23), S(35, 42), S(30, 53), S(38, 43), S(-46, 14),
        S(-91, 12), S(53, 32), S(13, 58), S(0, 70), S(33, 69), S(85, 60), S(35, 52), S(-28, 18),
        S(-55, 0), S(-25, 37), S(-32, 65), S(-59, 86), S(-55, 86), S(-33, 69), S(-50, 51), S(-117, 25),
        S(-63, -8), S(-28, 24), S(-33, 54), S(-57, 76), S(-54, 75), S(-28, 51), S(-55, 33), S(-127, 19),
        S(-38, -14), S(11, 10), S(-14, 33), S(-17, 45), S(-12, 43), S(-14, 32), S(0, 10), S(-56, 0),
        S(18, -29), S(16, -2), S(15, 10), S(-11, 21), S(-11, 22), S(0, 12), S(28, -6), S(4, -23),
        S(-4, -60), S(42, -42), S(26, -20), S(-43, -3), S(6, -21), S(-27, -9), S(20, -33), S(-5, -59)
    },

};


// Mobility Eval
// Each piece type can attack at most 28 squares in any given turn. In
// general, the more squares a piece attacks the better. But for example
// pieces like queens may get negative mobility in the opening to prevent
// early queen moves. The 4th index (index 5) of the mobilities array is 
// dedicated to king virtual mobility. That is putting a queen on a king sq
// and getting the mobility of the queen from there. This helps to position
// the king away from open rays
const int32_t mobilities[5][28] = {
    {
        S(0, 0), S(0, 0), S(113, 188), S(135, 184), S(157, 219), S(0, 0), S(194, 235), S(0, 0), S(166, 203), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(114, 127), S(125, 128), S(132, 159), S(145, 169), S(150, 177), S(161, 193), S(167, 199), S(172, 210), S(172, 214), S(175, 221), S(175, 218), S(174, 220), S(196, 211), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(188, 305), S(194, 318), S(200, 326), S(203, 330), S(202, 334), S(204, 339), S(209, 341), S(213, 345), S(218, 350), S(223, 352), S(225, 357), S(230, 361), S(230, 365), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(416, 361), S(406, 427), S(431, 474), S(434, 494), S(431, 553), S(436, 563), S(435, 581), S(438, 585), S(439, 596), S(441, 606), S(443, 608), S(445, 612), S(445, 619), S(444, 624), S(445, 630), S(445, 634), S(446, 636), S(447, 643), S(450, 640), S(452, 638), S(461, 631), S(461, 629), S(476, 621), S(511, 601), S(563, 572),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(57, -19), S(63, -19), S(45, 1), S(33, 3), S(27, -1), S(21, 0), S(15, 0), S(14, -1), S(2, 4), S(0, 0), S(-8, 3), S(-19, 5), S(-31, 7), S(-46, 7), S(-61, 7), S(-75, 7), S(-90, 5), S(-92, 0), S(-97, -3), S(-104, -8), S(-113, -14), S(-131, -18), S(-117, -32), S(-119, -36), S(-108, -49),
    },

};

// Bishop pair evaluation
const int32_t bishop_pair = S(19, 62);

// Passed pawn bonus
const int32_t passed_pawns[9]{
    S(14, -20), S(7, 6), S(-6, 34), S(-18, 46), S(-26, 28), S(-26, 31), S(-68, 25), S(33, -46), S(-120, -49),
};

// King Zone Attacks
// Bonus for pieces (not pawns or kings) which attack squares near the king
const int32_t inner_king_zone_attacks[4]{
    S(12, -6), S(18, -5), S(23, -7), S(14, 7),
};
// Attacking the ring 1 square away from the king square 
const int32_t outer_king_zone_attacks[4]{
    S(0, 1), S(0, 0), S(4, -3), S(2, 1),
};


// For a tapered evaluation
const int32_t game_phase_increment[6] = {0, 1, 1, 2, 4, 0};

// This is our HCE evaluation function. 
int32_t evaluate(const chess::Board& board) {

    int32_t score_array[2] = {0,0};
    int32_t phase = 0;

    // Get all piece bitboards for efficient looping
    chess::Bitboard wp = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
    chess::Bitboard wn = board.pieces(chess::PieceType::KNIGHT, chess::Color::WHITE);
    chess::Bitboard wb = board.pieces(chess::PieceType::BISHOP, chess::Color::WHITE);
    chess::Bitboard wr = board.pieces(chess::PieceType::ROOK, chess::Color::WHITE);
    chess::Bitboard wq = board.pieces(chess::PieceType::QUEEN, chess::Color::WHITE);
    chess::Bitboard wk = board.pieces(chess::PieceType::KING, chess::Color::WHITE);
    chess::Bitboard bp = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);
    chess::Bitboard bn = board.pieces(chess::PieceType::KNIGHT, chess::Color::BLACK);
    chess::Bitboard bb = board.pieces(chess::PieceType::BISHOP, chess::Color::BLACK);
    chess::Bitboard br = board.pieces(chess::PieceType::ROOK, chess::Color::BLACK);
    chess::Bitboard bq = board.pieces(chess::PieceType::QUEEN, chess::Color::BLACK);
    chess::Bitboard bk = board.pieces(chess::PieceType::KING, chess::Color::BLACK);
    chess::Bitboard all[] = {wp,wn,wb,wr,wq,wk,bp,bn,bb,br,bq,bk};

    int32_t whiteKingSq = board.kingSq(chess::Color::WHITE).index();
    int32_t blackKingSq = board.kingSq(chess::Color::BLACK).index();

    // uint64_t white_king_2_sq_mask = OUTER_2_SQ_RING_MASK[whiteKingSq];
    // uint64_t black_king_2_sq_mask = OUTER_2_SQ_RING_MASK[blackKingSq];
    uint64_t white_king_inner_sq_mask = chess::attacks::king(whiteKingSq).getBits();
    uint64_t black_king_inner_sq_mask = chess::attacks::king(blackKingSq).getBits();

    // A fast way of getting all the pieces 
    for (int32_t i = 0; i < 12; i++){        
        chess::Bitboard curr_bb = all[i];
        while (!curr_bb.empty()) {
            int16_t sq = curr_bb.pop();
            bool is_white = i < 6;
            int16_t j = is_white ? i : i-6;

            // Phase for tapered evaluation
            phase += game_phase_increment[j];

            // Piece square tables
            score_array[is_white ? 0 : 1] += PSQT[j][is_white ? sq ^ 56 : sq];

            // Mobilities for knight - queen, and king virtual mobility
            // King Zone
            if (j > 0){
                int attacks;
                uint64_t attacks_bb;
                switch (j)
                {
                    // knights
                    case 1:
                        attacks_bb = chess::attacks::knight(static_cast<chess::Square>(sq)).getBits();
                        attacks = count(attacks_bb);

                        /*
                        if (isWhite ? (sq == C3 && board.at(C2) == chess::Piece::WHITEPAWN) : (sq == C6 && board.at(C7) == chess::Piece::BLACKPAWN)){
                            trace.knight_blocking_c_pawn_malus[isWhite ? 0 : 1]++;
                        }
                            */

                        break;
                    // bishops
                    case 2:
                        attacks_bb = chess::attacks::bishop(static_cast<chess::Square>(sq), board.occ()).getBits();
                        attacks = count(attacks_bb);
                        break;
                    // rooks
                    case 3:
                        attacks_bb = chess::attacks::rook(static_cast<chess::Square>(sq), board.occ()).getBits();
                        attacks = count(attacks_bb);
                        break;
                    // queens
                    case 4:
                        attacks_bb = chess::attacks::queen(static_cast<chess::Square>(sq), board.occ()).getBits();
                        attacks = count(attacks_bb);
                        break;
                    // King Virtual Mobility
                    case 5:
                        attacks = chess::attacks::queen(static_cast<chess::Square>(sq), board.occ()).count();
                        break;

                    default:
                        break;
                }
                score_array[is_white ? 0 : 1] += mobilities[j-1][attacks];
                
                // Non king non pawn pieces
                if (j < 5){
                    score_array[is_white ? 0 : 1] += inner_king_zone_attacks[j-1]  * count((is_white ? black_king_inner_sq_mask : white_king_inner_sq_mask) & attacks_bb); 
                    //score_array[is_white ? 0 : 1] += outer_king_zone_attacks[j-1]  * count((is_white ? black_king_2_sq_mask : white_king_2_sq_mask) & attacks_bb); 
                }
            }

        }

    }

    // Bishop Pair
    if (wb.count() == 2) score_array[0] += bishop_pair;
    if (bb.count() == 2) score_array[1] += bishop_pair;

    // Passed pawn bonus
    int64_t wp_bits = wp.getBits();
    int64_t bp_bits = bp.getBits();

    score_array[0] += passed_pawns[count_white_passed_pawns(wp_bits, bp_bits)];
    score_array[1] += passed_pawns[count_black_passed_pawns(bp_bits, wp_bits)];

    int32_t stm = board.sideToMove() == Color::WHITE ? 0 : 1;
    int32_t score = score_array[stm] - score_array[stm^1];
    int32_t mg_score = (int32_t)unpack_mg(score);
    int32_t eg_score = (int32_t)unpack_eg(score);
    int32_t mg_phase = phase;
    if (mg_phase > 24) mg_phase = 24;
    int32_t eg_phase = 24 - mg_phase; 

    // Evaluation tapering, that is, interpolating mg and eg values depending on how many pieces
    // there are on the board. See here for more information: https://www.chessprogramming.org/Tapered_Eval
    return tempo.current + ((mg_score * mg_phase + eg_score * eg_phase) / 24);
}
