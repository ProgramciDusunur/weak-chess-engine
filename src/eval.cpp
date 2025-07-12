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
        S(130, 264), S(160, 255), S(139, 256), S(162, 209), S(154, 206), S(137, 219), S(73, 261), S(36, 273),
        S(62, 204), S(79, 213), S(107, 180), S(111, 158), S(120, 149), S(138, 142), S(123, 184), S(75, 179),
        S(51, 136), S(75, 127), S(74, 110), S(81, 98), S(96, 94), S(89, 99), S(92, 116), S(70, 114),
        S(43, 112), S(68, 112), S(69, 95), S(82, 91), S(85, 90), S(78, 92), S(79, 104), S(59, 95),
        S(44, 105), S(64, 110), S(65, 93), S(71, 100), S(80, 97), S(73, 93), S(92, 100), S(67, 92),
        S(46, 109), S(67, 113), S(59, 99), S(61, 109), S(73, 111), S(78, 100), S(97, 100), S(60, 93),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    },

    {
        S(12, 76), S(18, 141), S(58, 126), S(90, 119), S(123, 123), S(75, 97), S(49, 144), S(59, 52),
        S(114, 138), S(115, 127), S(104, 121), S(114, 123), S(95, 117), S(159, 103), S(122, 123), S(148, 122),
        S(113, 118), S(113, 119), S(155, 166), S(156, 171), S(187, 160), S(194, 151), S(125, 116), S(129, 112),
        S(115, 128), S(92, 135), S(139, 175), S(162, 176), S(129, 184), S(156, 178), S(85, 141), S(138, 123),
        S(103, 129), S(84, 123), S(126, 173), S(126, 174), S(138, 176), S(129, 166), S(97, 125), S(112, 120),
        S(86, 114), S(73, 117), S(114, 152), S(123, 164), S(133, 161), S(119, 145), S(89, 110), S(102, 114),
        S(96, 136), S(88, 118), S(67, 112), S(85, 113), S(82, 112), S(76, 108), S(94, 108), S(119, 144),
        S(69, 121), S(112, 127), S(78, 112), S(91, 114), S(95, 114), S(94, 105), S(110, 135), S(99, 107)
    },

    {
        S(109, 165), S(82, 173), S(86, 168), S(49, 179), S(64, 174), S(80, 164), S(101, 162), S(85, 163),
        S(112, 156), S(129, 155), S(119, 157), S(106, 159), S(121, 151), S(120, 153), S(119, 162), S(111, 155),
        S(124, 172), S(140, 159), S(136, 157), S(140, 149), S(129, 155), S(158, 158), S(142, 160), S(132, 173),
        S(120, 167), S(126, 164), S(130, 157), S(140, 168), S(137, 160), S(131, 163), S(127, 159), S(111, 168),
        S(121, 161), S(113, 164), S(116, 163), S(136, 159), S(132, 160), S(123, 157), S(118, 161), S(132, 153),
        S(126, 163), S(126, 162), S(127, 158), S(125, 156), S(127, 159), S(129, 157), S(131, 153), S(143, 155),
        S(136, 167), S(135, 151), S(135, 143), S(121, 155), S(129, 153), S(132, 149), S(144, 158), S(140, 150),
        S(130, 157), S(139, 163), S(132, 153), S(118, 158), S(125, 158), S(119, 165), S(133, 151), S(144, 142)
    },

    {
        S(109, 179), S(91, 188), S(89, 198), S(85, 196), S(96, 190), S(118, 183), S(117, 182), S(132, 177),
        S(92, 183), S(84, 196), S(99, 201), S(113, 193), S(92, 198), S(120, 186), S(114, 181), S(141, 166),
        S(82, 182), S(102, 183), S(98, 185), S(100, 183), S(124, 174), S(115, 172), S(153, 164), S(124, 160),
        S(76, 184), S(85, 180), S(84, 188), S(92, 183), S(86, 175), S(91, 169), S(93, 168), S(96, 163),
        S(62, 177), S(60, 180), S(68, 181), S(79, 178), S(73, 178), S(59, 175), S(74, 166), S(69, 163),
        S(56, 175), S(60, 172), S(67, 171), S(69, 174), S(69, 171), S(66, 163), S(87, 149), S(74, 152),
        S(57, 169), S(66, 169), S(79, 169), S(80, 169), S(80, 164), S(71, 161), S(83, 154), S(66, 160),
        S(73, 172), S(75, 171), S(83, 175), S(85, 171), S(84, 166), S(67, 170), S(81, 163), S(71, 163)
    },

    {
        S(335, 546), S(321, 561), S(336, 579), S(363, 561), S(350, 568), S(349, 572), S(399, 517), S(361, 543),
        S(348, 538), S(326, 565), S(324, 595), S(315, 608), S(303, 629), S(345, 584), S(347, 568), S(388, 558),
        S(354, 544), S(348, 552), S(346, 582), S(343, 588), S(351, 591), S(371, 575), S(383, 546), S(374, 548),
        S(338, 558), S(342, 564), S(335, 574), S(331, 590), S(331, 591), S(341, 580), S(350, 577), S(348, 566),
        S(345, 550), S(334, 567), S(334, 567), S(341, 577), S(335, 574), S(336, 563), S(343, 558), S(351, 555),
        S(340, 537), S(346, 544), S(341, 554), S(337, 549), S(339, 554), S(343, 546), S(352, 530), S(350, 537),
        S(350, 532), S(348, 530), S(355, 525), S(352, 534), S(351, 536), S(350, 502), S(357, 481), S(382, 468),
        S(346, 531), S(343, 527), S(348, 527), S(353, 536), S(350, 515), S(326, 517), S(347, 502), S(354, 502)
    },

    {
        S(33, -96), S(22, -43), S(70, -35), S(-60, 12), S(-20, -4), S(-8, 0), S(20, -10), S(118, -114),
        S(-91, 0), S(11, 27), S(-19, 40), S(92, 23), S(35, 42), S(30, 53), S(37, 43), S(-47, 15),
        S(-91, 12), S(52, 32), S(12, 58), S(0, 70), S(33, 69), S(85, 60), S(35, 52), S(-28, 18),
        S(-55, 0), S(-25, 37), S(-32, 65), S(-59, 86), S(-55, 86), S(-33, 69), S(-50, 51), S(-118, 25),
        S(-63, -8), S(-28, 24), S(-33, 54), S(-57, 76), S(-54, 75), S(-29, 51), S(-55, 33), S(-127, 19),
        S(-38, -14), S(11, 10), S(-14, 33), S(-17, 45), S(-12, 43), S(-14, 32), S(0, 10), S(-56, 0),
        S(18, -29), S(16, -2), S(15, 10), S(-11, 21), S(-11, 22), S(0, 12), S(28, -6), S(4, -23),
        S(-4, -60), S(42, -42), S(26, -20), S(-43, -3), S(6, -21), S(-27, -9), S(20, -33), S(-6, -59)
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
        S(0, 0), S(0, 0), S(118, 194), S(136, 184), S(155, 216), S(0, 0), S(191, 230), S(0, 0), S(165, 202), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(113, 125), S(124, 127), S(131, 158), S(144, 167), S(149, 176), S(160, 192), S(167, 197), S(171, 209), S(172, 213), S(174, 220), S(174, 217), S(173, 218), S(195, 210), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(94, 179), S(100, 191), S(106, 199), S(109, 203), S(108, 207), S(110, 212), S(115, 215), S(119, 218), S(124, 223), S(129, 225), S(131, 231), S(136, 234), S(136, 238), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(414, 360), S(404, 426), S(429, 473), S(432, 492), S(429, 552), S(434, 561), S(432, 579), S(435, 584), S(437, 595), S(438, 605), S(440, 607), S(443, 610), S(442, 617), S(442, 623), S(443, 628), S(442, 632), S(443, 635), S(444, 641), S(447, 639), S(449, 637), S(458, 630), S(459, 627), S(473, 620), S(508, 599), S(560, 571),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(57, -19), S(63, -19), S(45, 1), S(33, 3), S(27, -1), S(21, 0), S(15, 0), S(14, -1), S(2, 4), S(0, 0), S(-8, 4), S(-18, 5), S(-31, 7), S(-45, 8), S(-61, 7), S(-75, 7), S(-90, 5), S(-92, 0), S(-97, -3), S(-104, -8), S(-113, -14), S(-131, -18), S(-117, -32), S(-119, -36), S(-108, -49),
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
    S(0, 1), S(0, -1), S(4, -3), S(2, 1),
};

// Rook on open file bonus
const int32_t rook_on_open_file = S(168, 242);

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
                        if ((is_white ? (WHITE_AHEAD_MASK[sq] & wp & bp) : (BLACK_AHEAD_MASK[sq] & wp & bp)) == 0ull)
                            score_array[is_white ? 0 : 1] += rook_on_open_file;
                            
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
