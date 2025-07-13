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
        S(62, 150), S(76, 144), S(67, 145), S(79, 119), S(73, 119), S(65, 127), S(35, 146), S(13, 154),
        S(69, 132), S(84, 138), S(105, 121), S(107, 126), S(117, 101), S(152, 110), S(138, 140), S(99, 124),
        S(54, 127), S(79, 126), S(78, 114), S(83, 98), S(100, 98), S(95, 107), S(97, 116), S(73, 105),
        S(44, 114), S(72, 121), S(72, 108), S(85, 103), S(87, 101), S(82, 106), S(83, 110), S(60, 96),
        S(45, 111), S(66, 119), S(71, 108), S(72, 109), S(82, 111), S(81, 109), S(97, 106), S(68, 94),
        S(52, 112), S(68, 119), S(61, 113), S(63, 116), S(74, 122), S(86, 111), S(94, 108), S(62, 93),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    },
    {
        S(16, 79), S(26, 138), S(57, 126), S(85, 116), S(122, 119), S(69, 97), S(56, 138), S(57, 58),
        S(118, 139), S(113, 123), S(103, 119), S(115, 121), S(95, 116), S(156, 101), S(121, 118), S(153, 123),
        S(113, 114), S(113, 119), S(155, 169), S(156, 174), S(186, 162), S(198, 152), S(124, 116), S(131, 109),
        S(113, 128), S(92, 135), S(142, 178), S(164, 179), S(131, 186), S(159, 181), S(86, 141), S(137, 120),
        S(102, 127), S(83, 126), S(128, 176), S(127, 177), S(139, 179), S(131, 169), S(100, 124), S(112, 118),
        S(83, 112), S(73, 117), S(114, 156), S(121, 166), S(132, 164), S(121, 149), S(91, 109), S(100, 111),
        S(95, 138), S(82, 115), S(64, 113), S(83, 112), S(80, 110), S(74, 107), S(89, 102), S(119, 143),
        S(74, 128), S(107, 126), S(74, 111), S(89, 113), S(93, 113), S(87, 102), S(108, 132), S(101, 117)
    },
    {
        S(107, 167), S(82, 176), S(86, 168), S(49, 178), S(65, 173), S(79, 165), S(100, 166), S(85, 163),
        S(112, 159), S(128, 156), S(119, 159), S(107, 160), S(122, 152), S(117, 156), S(119, 162), S(107, 160),
        S(125, 173), S(139, 161), S(138, 157), S(139, 152), S(129, 157), S(159, 160), S(140, 163), S(132, 176),
        S(120, 168), S(128, 165), S(131, 159), S(141, 170), S(137, 162), S(131, 165), S(129, 161), S(111, 169),
        S(122, 165), S(113, 166), S(118, 165), S(137, 162), S(133, 161), S(125, 159), S(119, 162), S(133, 155),
        S(123, 166), S(127, 164), S(126, 161), S(123, 161), S(125, 163), S(130, 157), S(132, 154), S(142, 156),
        S(129, 171), S(131, 152), S(133, 145), S(120, 157), S(127, 154), S(128, 150), S(141, 154), S(133, 152),
        S(129, 157), S(134, 166), S(127, 156), S(118, 161), S(123, 159), S(114, 166), S(128, 153), S(144, 139)
    },
    {
        S(182, 301), S(165, 310), S(163, 320), S(159, 317), S(170, 312), S(191, 305), S(188, 305), S(204, 299),
        S(166, 305), S(157, 317), S(172, 322), S(186, 314), S(166, 319), S(193, 306), S(191, 300), S(217, 288),
        S(156, 304), S(178, 303), S(172, 304), S(173, 303), S(198, 294), S(193, 291), S(228, 284), S(200, 281),
        S(150, 306), S(160, 302), S(160, 308), S(166, 303), S(161, 296), S(169, 289), S(169, 291), S(171, 286),
        S(138, 301), S(137, 302), S(144, 302), S(154, 299), S(150, 298), S(137, 296), S(150, 289), S(145, 287),
        S(135, 297), S(137, 294), S(145, 292), S(147, 294), S(149, 291), S(148, 284), S(164, 270), S(152, 274),
        S(137, 290), S(143, 291), S(155, 290), S(156, 288), S(157, 283), S(151, 281), S(162, 273), S(143, 281),
        S(151, 295), S(152, 292), S(157, 296), S(158, 292), S(158, 288), S(147, 292), S(157, 286), S(150, 284)
    },
    {
        S(326, 558), S(316, 570), S(330, 587), S(356, 571), S(342, 581), S(347, 578), S(393, 527), S(353, 556),
        S(345, 549), S(322, 572), S(320, 601), S(310, 618), S(297, 638), S(338, 591), S(342, 578), S(383, 570),
        S(350, 553), S(343, 562), S(343, 586), S(339, 594), S(345, 598), S(367, 582), S(376, 558), S(367, 561),
        S(332, 570), S(339, 573), S(332, 582), S(327, 598), S(327, 599), S(337, 587), S(346, 586), S(342, 577),
        S(342, 559), S(330, 576), S(332, 572), S(337, 584), S(332, 583), S(333, 570), S(339, 565), S(346, 566),
        S(335, 546), S(343, 552), S(338, 563), S(334, 558), S(337, 562), S(339, 554), S(349, 538), S(345, 546),
        S(344, 544), S(344, 540), S(351, 534), S(348, 541), S(347, 545), S(345, 512), S(353, 489), S(375, 482),
        S(340, 543), S(337, 540), S(342, 537), S(346, 545), S(343, 527), S(321, 526), S(341, 512), S(351, 511)
    },
    {
        S(34, -100), S(26, -48), S(65, -36), S(-59, 11), S(-28, -3), S(-10, -2), S(37, -16), S(137, -121),
        S(-89, -6), S(5, 25), S(-15, 36), S(83, 24), S(35, 41), S(31, 50), S(49, 36), S(-26, 6),
        S(-86, 5), S(55, 28), S(15, 56), S(-1, 69), S(37, 69), S(93, 59), S(42, 47), S(-20, 11),
        S(-52, -4), S(-23, 34), S(-28, 63), S(-58, 87), S(-52, 87), S(-32, 69), S(-49, 48), S(-112, 20),
        S(-68, -8), S(-29, 23), S(-38, 54), S(-63, 79), S(-56, 77), S(-31, 52), S(-56, 32), S(-128, 17),
        S(-38, -16), S(9, 9), S(-17, 35), S(-21, 47), S(-16, 46), S(-18, 34), S(-3, 11), S(-58, 0),
        S(20, -28), S(16, -1), S(13, 10), S(-10, 20), S(-12, 23), S(0, 13), S(21, -3), S(2, -21),
        S(-3, -63), S(39, -43), S(24, -21), S(-41, -6), S(9, -25), S(-31, -8), S(19, -34), S(-3, -59)
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
        S(0, 0), S(0, 0), S(113, 195), S(133, 191), S(157, 225), S(0, 0), S(192, 237), S(0, 0), S(165, 205), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },
    {
        S(0, 0), S(115, 134), S(126, 132), S(132, 164), S(146, 174), S(151, 183), S(161, 199), S(168, 205), S(172, 216), S(172, 220), S(175, 226), S(175, 223), S(174, 224), S(194, 218), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },
    {
        S(0, 0), S(0, 0), S(197, 304), S(203, 322), S(206, 330), S(207, 336), S(206, 341), S(207, 346), S(211, 348), S(215, 351), S(220, 355), S(224, 359), S(226, 365), S(231, 368), S(230, 372), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },
    {
        S(0, 0), S(0, 0), S(0, 0), S(425, 363), S(409, 444), S(437, 484), S(439, 506), S(436, 562), S(441, 571), S(439, 590), S(442, 596), S(444, 606), S(445, 617), S(446, 619), S(448, 624), S(448, 632), S(447, 637), S(448, 642), S(447, 648), S(447, 650), S(448, 657), S(450, 655), S(452, 654), S(461, 647), S(461, 645), S(475, 637), S(510, 617), S(565, 587),
    },
    {
        S(0, 0), S(0, 0), S(0, 0), S(50, -10), S(55, -15), S(42, 4), S(30, 3), S(25, -1), S(19, 1), S(14, 0), S(13, 0), S(3, 5), S(1, 1), S(-6, 5), S(-16, 6), S(-29, 9), S(-42, 8), S(-58, 7), S(-72, 7), S(-87, 4), S(-89, -1), S(-96, -5), S(-101, -10), S(-112, -17), S(-126, -22), S(-113, -36), S(-117, -40), S(-103, -54),
    },
};


// Bishop pair evaluation
const int32_t bishop_pair = S(20, 62);

// Passed pawn bonus
const int32_t passed_pawns[64] = {
    S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    S(62, 150), S(76, 144), S(67, 145), S(79, 119), S(73, 119), S(65, 127), S(35, 146), S(13, 154),
    S(16, 132), S(27, 126), S(17, 101), S(5, 61), S(9, 79), S(2, 92), S(-23, 101), S(-49, 131),
    S(11, 64), S(6, 57), S(15, 40), S(11, 37), S(0, 35), S(8, 38), S(-20, 59), S(-18, 70),
    S(-3, 33), S(-13, 23), S(-19, 16), S(-13, 11), S(-20, 14), S(-12, 15), S(-21, 33), S(-12, 35),
    S(-9, -2), S(-18, 0), S(-27, 0), S(-24, -5), S(-23, -6), S(-17, -7), S(-20, 13), S(-3, 3),
    S(-17, -2), S(-11, -3), S(-19, -2), S(-26, -3), S(-14, -17), S(-11, -11), S(5, -6), S(-7, 2),
    S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
};


// King Zone Attacks
// Bonus for pieces (not pawns or kings) which attack squares near the king
const int32_t inner_king_zone_attacks[4] = {
    S(12, -6), S(18, -4), S(23, -7), S(14, 7),
};
// Attacking the ring 1 square away from the king square 
const int32_t outer_king_zone_attacks[4] = {
    S(0, 1), S(0, 0), S(3, -3), S(3, 1),
};


// Doubled pawn penalty
const int32_t doubled_pawn_penalty[8] = {
    S(-7, -41), S(-8, -25), S(-10, -27), S(-11, -11), S(-11, -21), S(-11, -25), S(-8, -23), S(-15, -35),
};


// Pawn directly in front
const int32_t pawn_front_square[6] = {
    S(-10, -16), S(9, 10), S(8, 4), S(-10, -7), S(0, -11), S(12, -8),
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

            // Whether there is a pawn right in front of us
            if (is_white ? (WHITE_FRONT_MASK[sq] & wp) : (BLACK_FRONT_MASK[sq] & bp))
                score_array[is_white ? 0 : 1] += pawn_front_square[j];

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
