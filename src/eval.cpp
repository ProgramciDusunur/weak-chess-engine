#include <stdint.h>

#include "chess.hpp"
#include "packing.hpp"
#include "eval.hpp"
#include "defaults.hpp"
#include "bitboard.hpp"

using namespace chess;
using namespace std;

// Entire evaluation function is tuned with non other than Gedas' Texel tuner <3
// https://github.com/GediminasMasaitis/texel-tuner

// Piece Square Tables (which includes the material value of each piece)
// Each piece type has its own piece square table, whiched it used
// to evaluate its positioning on the board
const int32_t PSQT[6][64] = {
    {
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
        S(58, 168), S(109, 155), S(75, 172), S(83, 136), S(79, 111), S(71, 130), S(38, 148), S(18, 155),
        S(95, 155), S(113, 165), S(103, 154), S(102, 146), S(116, 101), S(151, 114), S(148, 147), S(106, 128),
        S(87, 133), S(115, 133), S(102, 119), S(77, 111), S(102, 94), S(98, 107), S(104, 116), S(83, 102),
        S(76, 116), S(99, 124), S(96, 107), S(84, 109), S(89, 98), S(87, 104), S(90, 109), S(70, 93),
        S(90, 106), S(115, 112), S(94, 106), S(61, 115), S(86, 110), S(84, 108), S(110, 103), S(80, 88),
        S(84, 105), S(126, 107), S(105, 101), S(75, 116), S(77, 123), S(91, 111), S(118, 100), S(73, 85),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    },
    {
        S(12, 85), S(22, 142), S(54, 128), S(82, 118), S(118, 122), S(63, 99), S(55, 139), S(63, 61),
        S(112, 143), S(112, 125), S(101, 119), S(116, 120), S(97, 114), S(155, 100), S(126, 119), S(151, 124),
        S(112, 116), S(111, 118), S(156, 168), S(158, 173), S(189, 161), S(203, 150), S(125, 114), S(132, 109),
        S(113, 129), S(90, 133), S(143, 178), S(164, 179), S(134, 185), S(162, 180), S(87, 139), S(138, 121),
        S(102, 128), S(82, 125), S(129, 177), S(128, 177), S(140, 180), S(132, 168), S(99, 124), S(113, 119),
        S(85, 114), S(73, 117), S(116, 155), S(126, 168), S(137, 164), S(122, 150), S(90, 109), S(102, 113),
        S(100, 141), S(88, 118), S(67, 114), S(84, 112), S(82, 110), S(81, 107), S(99, 106), S(124, 146),
        S(83, 132), S(113, 130), S(80, 113), S(93, 113), S(97, 113), S(99, 103), S(113, 138), S(112, 118)
    },
    {
        S(108, 168), S(80, 178), S(82, 170), S(44, 181), S(62, 176), S(72, 169), S(105, 166), S(79, 165),
        S(106, 161), S(128, 157), S(119, 160), S(106, 161), S(121, 153), S(121, 156), S(116, 165), S(112, 160),
        S(123, 175), S(138, 162), S(136, 158), S(140, 152), S(129, 158), S(159, 161), S(142, 163), S(132, 177),
        S(118, 170), S(128, 165), S(131, 160), S(142, 170), S(138, 163), S(133, 165), S(129, 162), S(114, 170),
        S(123, 166), S(115, 167), S(118, 166), S(138, 164), S(134, 162), S(126, 160), S(120, 163), S(135, 156),
        S(126, 168), S(129, 165), S(129, 162), S(128, 161), S(129, 163), S(132, 158), S(134, 156), S(144, 158),
        S(140, 170), S(136, 154), S(138, 147), S(124, 157), S(131, 155), S(138, 150), S(151, 158), S(142, 154),
        S(131, 158), S(143, 166), S(134, 158), S(122, 161), S(128, 160), S(125, 169), S(134, 154), S(149, 142)
    },
    {
        S(177, 303), S(164, 310), S(162, 320), S(159, 316), S(170, 311), S(194, 306), S(187, 307), S(200, 301),
        S(164, 307), S(157, 318), S(173, 322), S(187, 314), S(169, 318), S(203, 302), S(196, 298), S(221, 287),
        S(153, 305), S(176, 304), S(169, 306), S(172, 303), S(196, 294), S(197, 290), S(226, 284), S(199, 282),
        S(146, 308), S(157, 303), S(159, 308), S(164, 303), S(161, 296), S(171, 289), S(168, 292), S(169, 288),
        S(133, 302), S(133, 303), S(142, 302), S(152, 299), S(149, 298), S(139, 296), S(151, 289), S(146, 288),
        S(129, 300), S(134, 295), S(142, 293), S(144, 295), S(147, 292), S(145, 285), S(166, 270), S(150, 275),
        S(131, 292), S(140, 292), S(155, 290), S(156, 289), S(157, 283), S(154, 279), S(168, 271), S(143, 281),
        S(147, 294), S(149, 293), S(158, 297), S(163, 291), S(162, 288), S(148, 290), S(159, 283), S(151, 281)
    },
    {
        S(328, 563), S(319, 574), S(334, 591), S(358, 576), S(347, 583), S(359, 578), S(405, 525), S(360, 558),
        S(347, 553), S(327, 573), S(326, 602), S(315, 620), S(305, 639), S(355, 587), S(356, 578), S(389, 575),
        S(353, 555), S(346, 564), S(346, 588), S(344, 594), S(352, 599), S(377, 582), S(386, 559), S(373, 566),
        S(336, 570), S(343, 573), S(338, 581), S(333, 597), S(334, 599), S(344, 589), S(351, 589), S(349, 580),
        S(346, 561), S(335, 577), S(337, 573), S(342, 585), S(338, 584), S(339, 574), S(345, 570), S(353, 569),
        S(342, 546), S(348, 554), S(343, 564), S(340, 559), S(342, 565), S(345, 558), S(356, 541), S(352, 550),
        S(351, 543), S(350, 541), S(358, 535), S(353, 543), S(353, 546), S(354, 516), S(359, 497), S(375, 488),
        S(345, 544), S(345, 539), S(350, 537), S(355, 548), S(353, 527), S(332, 528), S(345, 516), S(356, 512)
    },
    {
        S(75, -122), S(72, -70), S(109, -56), S(-26, -6), S(-35, 3), S(-20, 2), S(40, -13), S(156, -120),
        S(-53, -29), S(42, 2), S(21, 15), S(108, 6), S(23, 49), S(19, 57), S(39, 43), S(-16, 10),
        S(-62, -16), S(80, 7), S(37, 35), S(17, 50), S(17, 77), S(78, 66), S(38, 53), S(-21, 18),
        S(-33, -25), S(-4, 13), S(-11, 42), S(-49, 68), S(-76, 97), S(-46, 78), S(-52, 55), S(-110, 26),
        S(-58, -28), S(-17, 1), S(-27, 31), S(-55, 57), S(-77, 88), S(-45, 63), S(-60, 40), S(-126, 24),
        S(-28, -36), S(14, -11), S(-15, 13), S(-20, 25), S(-31, 58), S(-28, 44), S(-7, 20), S(-54, 7),
        S(28, -49), S(27, -22), S(19, -10), S(0, -1), S(-22, 34), S(-7, 24), S(22, 4), S(4, -12),
        S(12, -84), S(43, -65), S(27, -43), S(-34, -25), S(-3, -9), S(-32, 1), S(17, -22), S(0, -48)
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
        S(0, 0), S(0, 0), S(113, 193), S(139, 189), S(161, 224), S(0, 0), S(197, 239), S(0, 0), S(168, 206), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },
    {
        S(0, 0), S(117, 135), S(131, 131), S(136, 163), S(148, 173), S(154, 182), S(165, 198), S(171, 204), S(176, 215), S(176, 219), S(179, 226), S(180, 223), S(179, 224), S(199, 219), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },
    {
        S(0, 0), S(0, 0), S(196, 305), S(201, 320), S(207, 328), S(210, 333), S(211, 339), S(212, 345), S(217, 348), S(222, 352), S(227, 357), S(231, 360), S(234, 366), S(240, 369), S(240, 372), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },
    {
        S(0, 0), S(0, 0), S(0, 0), S(430, 355), S(416, 441), S(442, 481), S(443, 502), S(440, 560), S(444, 571), S(442, 590), S(445, 595), S(447, 605), S(448, 617), S(450, 619), S(452, 624), S(451, 632), S(451, 638), S(452, 644), S(451, 649), S(452, 652), S(453, 659), S(456, 657), S(458, 657), S(467, 650), S(467, 649), S(481, 642), S(517, 620), S(574, 589),
    },
    {
        S(0, 0), S(0, 0), S(0, 0), S(46, -7), S(49, -11), S(32, 3), S(25, 4), S(23, -1), S(19, 0), S(14, 0), S(13, 0), S(5, 5), S(5, 2), S(-3, 5), S(-12, 7), S(-23, 8), S(-36, 8), S(-51, 7), S(-62, 7), S(-76, 4), S(-75, -1), S(-78, -6), S(-83, -11), S(-91, -17), S(-103, -22), S(-85, -36), S(-96, -40), S(-76, -53),
    },
};

// Bishop pair evaluation
const int32_t bishop_pair = S(20, 62);

// Passed pawn bonus
const int32_t passed_pawns[64] = {
    S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    S(58, 168), S(109, 155), S(75, 172), S(83, 136), S(79, 111), S(71, 130), S(38, 148), S(18, 155),
    S(17, 130), S(33, 122), S(23, 98), S(4, 62), S(12, 75), S(13, 89), S(-17, 98), S(-44, 126),
    S(11, 66), S(10, 56), S(17, 41), S(13, 36), S(3, 35), S(14, 37), S(-5, 55), S(-15, 69),
    S(-2, 34), S(-10, 23), S(-19, 18), S(-11, 11), S(-16, 13), S(-9, 15), S(-8, 29), S(-6, 32),
    S(-9, -1), S(-17, 1), S(-26, 1), S(-25, -5), S(-20, -7), S(-10, -8), S(0, 7), S(2, 1),
    S(-15, -2), S(-11, -1), S(-21, 0), S(-25, -3), S(-9, -19), S(0, -15), S(16, -9), S(-1, 1),
    S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
};


// King Zone Attacks
// Bonus for pieces (not pawns or kings) which attack squares near the king
const int32_t inner_king_zone_attacks[4] = {
    S(9, -5), S(17, -4), S(20, -6), S(12, 7),
};
// Attacking the ring 1 square away from the king square 
const int32_t outer_king_zone_attacks[4] = {
    S(0, 1), S(0, 0), S(3, -3), S(3, 0),
};


// Doubled pawn penalty
const int32_t doubled_pawn_penalty[8] = {
    S(-11, -49), S(-18, -33), S(-15, -36), S(-15, -18), S(-15, -29), S(-15, -34), S(-19, -31), S(-19, -43),
};


// Pawn storm
const int32_t pawn_storm[64] = {
    S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    S(13, -57), S(-66, -39), S(0, -87), S(-3, -51), S(-51, 55), S(-39, 0), S(-21, 4), S(-50, 6),
    S(-26, -31), S(-28, -36), S(2, -45), S(7, -28), S(-8, 18), S(-29, 2), S(-49, -3), S(-55, 4),
    S(-33, -10), S(-35, -12), S(-25, -10), S(6, -16), S(-8, 13), S(-12, 4), S(-36, 12), S(-52, 18),
    S(-33, -5), S(-27, -6), S(-23, -1), S(2, -9), S(-3, 6), S(-14, 7), S(-30, 9), S(-49, 17),
    S(-47, 2), S(-48, 3), S(-24, 0), S(15, -6), S(-11, 1), S(-15, 2), S(-60, 16), S(-67, 24),
    S(-39, 8), S(-58, 10), S(-45, 10), S(-11, -1), S(-27, 3), S(-34, 7), S(-72, 27), S(-60, 39),
    S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
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

    uint64_t not_kingside_w_mask = NOT_KINGSIDE_HALF_MASK[whiteKingSq];
    uint64_t not_kingside_b_mask = NOT_KINGSIDE_HALF_MASK[blackKingSq];

    uint64_t white_king_2_sq_mask = OUTER_2_SQ_RING_MASK[whiteKingSq];
    uint64_t black_king_2_sq_mask = OUTER_2_SQ_RING_MASK[blackKingSq];
    uint64_t white_king_inner_sq_mask = chess::attacks::king(whiteKingSq).getBits();
    uint64_t black_king_inner_sq_mask = chess::attacks::king(blackKingSq).getBits();

    // Keep track of rooks on open files for updating after loop
    // int32_t num_w_rooks_on_op_file = 0;
    // int32_t num_b_rooks_on_op_file = 0;

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
                int32_t attacks = 0;
                uint64_t attacks_bb = 0ull;
                switch (j)
                {
                    // knights
                    case 1:
                        attacks_bb = chess::attacks::knight(static_cast<chess::Square>(sq)).getBits();
                        attacks = count(attacks_bb);
                        break;
                    // bishops
                    case 2:
                        attacks_bb = chess::attacks::bishop(static_cast<chess::Square>(sq), board.occ()).getBits();
                        attacks = count(attacks_bb);
                        break;
                    // rooks
                    case 3:
                        // Rook on open file
                        /*
                        if ((is_white ? (WHITE_AHEAD_MASK[sq] & wp & bp) : (BLACK_AHEAD_MASK[sq] & wp & bp)) == 0ull){
                            if (is_white) num_w_rooks_on_op_file++;
                            else num_b_rooks_on_op_file++;
                        }
                        */

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
                    score_array[is_white ? 0 : 1] += outer_king_zone_attacks[j-1]  * count((is_white ? black_king_2_sq_mask : white_king_2_sq_mask) & attacks_bb); 
                }
            }

            // Pawns
            else {
                // Doubled pawns
                // Note that for pawns to be considered "doubled", they need not be directly in front
                // of another pawn
                uint64_t front_mask = is_white ? WHITE_AHEAD_MASK[sq] : BLACK_AHEAD_MASK[sq];
                uint64_t our_pawn_bb = is_white ? wp.getBits() : bp.getBits();
                if (front_mask & our_pawn_bb)
                    score_array[is_white ? 0 : 1] += doubled_pawn_penalty[is_white ? 7 - sq % 8 : sq % 8];

                // Passed pawn
                if (is_white ? is_white_passed_pawn(sq, bp.getBits()): is_black_passed_pawn(sq, wp.getBits())){
                    score_array[is_white ? 0 : 1] += passed_pawns[is_white ? sq ^ 56 : sq];
                }

                // Pawn storm
                if (is_white ? (not_kingside_w_mask & (1ull << sq)) : (not_kingside_b_mask & (1ull << sq))){
                    score_array[is_white ? 0 : 1] += pawn_storm[is_white ? sq ^ 56 : sq];
                }
            }

        }

    }

    // Rooks on open files
    /*
    if (num_w_rooks_on_op_file == 1) score_array[0] += rook_open_file[0];
    if (num_w_rooks_on_op_file == 2) score_array[0] += rook_open_file[1];
    if (num_b_rooks_on_op_file == 1) score_array[1] += rook_open_file[0];
    if (num_b_rooks_on_op_file == 2) score_array[1] += rook_open_file[1];
    */

    // Bishop Pair
    if (wb.count() == 2) score_array[0] += bishop_pair;
    if (bb.count() == 2) score_array[1] += bishop_pair;

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
