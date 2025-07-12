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
        S(128, 266), S(158, 256), S(138, 257), S(160, 210), S(152, 208), S(136, 221), S(72, 262), S(35, 275),
        S(62, 204), S(78, 213), S(107, 180), S(111, 159), S(120, 150), S(137, 142), S(122, 185), S(75, 180),
        S(51, 136), S(75, 127), S(74, 111), S(81, 98), S(96, 94), S(89, 99), S(92, 116), S(69, 114),
        S(43, 112), S(68, 112), S(69, 95), S(82, 91), S(85, 90), S(78, 92), S(79, 104), S(59, 96),
        S(44, 105), S(64, 110), S(65, 93), S(71, 100), S(80, 97), S(73, 94), S(91, 101), S(67, 92),
        S(46, 109), S(67, 113), S(59, 99), S(61, 109), S(73, 111), S(78, 100), S(97, 100), S(60, 93),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    },

    {
        S(8, 79), S(16, 142), S(58, 126), S(89, 119), S(123, 123), S(73, 98), S(47, 145), S(55, 55),
        S(113, 139), S(115, 126), S(104, 120), S(114, 122), S(95, 116), S(159, 102), S(122, 123), S(148, 123),
        S(113, 118), S(113, 119), S(154, 166), S(156, 170), S(187, 160), S(194, 151), S(126, 115), S(129, 111),
        S(115, 127), S(92, 134), S(139, 175), S(162, 176), S(129, 184), S(156, 178), S(86, 139), S(139, 122),
        S(103, 128), S(84, 123), S(126, 173), S(126, 174), S(138, 176), S(129, 166), S(98, 124), S(112, 119),
        S(86, 113), S(74, 116), S(114, 151), S(122, 164), S(133, 161), S(119, 145), S(90, 109), S(102, 113),
        S(95, 137), S(88, 117), S(67, 111), S(85, 111), S(83, 111), S(77, 107), S(95, 107), S(119, 144),
        S(69, 121), S(112, 127), S(78, 111), S(91, 114), S(96, 113), S(94, 104), S(110, 135), S(99, 107)
    },

    {
        S(106, 167), S(81, 174), S(84, 169), S(47, 180), S(61, 175), S(78, 165), S(100, 163), S(82, 165),
        S(111, 156), S(128, 156), S(119, 158), S(104, 160), S(120, 151), S(120, 154), S(118, 163), S(111, 156),
        S(123, 173), S(139, 159), S(135, 158), S(140, 150), S(129, 155), S(157, 158), S(141, 161), S(132, 173),
        S(120, 167), S(125, 165), S(129, 157), S(140, 168), S(136, 160), S(130, 163), S(127, 160), S(111, 169),
        S(121, 161), S(113, 164), S(116, 163), S(136, 159), S(132, 159), S(123, 157), S(117, 161), S(132, 153),
        S(126, 163), S(126, 162), S(127, 158), S(124, 156), S(127, 159), S(129, 157), S(131, 153), S(142, 155),
        S(135, 167), S(135, 151), S(135, 143), S(121, 155), S(128, 153), S(132, 149), S(144, 158), S(139, 150),
        S(130, 157), S(139, 163), S(132, 153), S(117, 159), S(124, 158), S(119, 165), S(133, 151), S(144, 142)
    },

    {
        S(159, 226), S(143, 234), S(140, 245), S(137, 243), S(147, 238), S(172, 229), S(169, 228), S(182, 223),
        S(140, 231), S(132, 244), S(147, 249), S(161, 241), S(140, 247), S(168, 234), S(162, 228), S(190, 213),
        S(130, 229), S(151, 231), S(146, 233), S(148, 232), S(172, 222), S(163, 220), S(201, 211), S(172, 208),
        S(124, 231), S(134, 228), S(132, 236), S(140, 232), S(134, 224), S(139, 217), S(141, 216), S(144, 211),
        S(110, 224), S(108, 227), S(116, 229), S(127, 227), S(121, 226), S(107, 224), S(122, 214), S(117, 211),
        S(104, 223), S(107, 221), S(115, 219), S(117, 222), S(116, 220), S(114, 212), S(134, 198), S(121, 201),
        S(105, 218), S(113, 218), S(127, 217), S(127, 218), S(128, 213), S(118, 210), S(131, 203), S(114, 208),
        S(121, 221), S(122, 220), S(130, 224), S(132, 220), S(131, 216), S(114, 219), S(128, 212), S(119, 212)
    },

    {
        S(328, 550), S(315, 564), S(332, 581), S(359, 563), S(346, 571), S(344, 574), S(393, 520), S(355, 547),
        S(345, 539), S(323, 566), S(320, 596), S(311, 609), S(300, 630), S(341, 585), S(343, 570), S(384, 559),
        S(350, 546), S(344, 553), S(342, 583), S(340, 589), S(347, 592), S(367, 575), S(380, 547), S(370, 549),
        S(334, 559), S(339, 565), S(331, 575), S(328, 591), S(327, 592), S(337, 581), S(346, 578), S(344, 567),
        S(342, 551), S(330, 568), S(331, 568), S(337, 578), S(332, 575), S(333, 564), S(339, 559), S(347, 556),
        S(337, 538), S(343, 545), S(338, 555), S(333, 551), S(336, 556), S(339, 548), S(348, 532), S(347, 538),
        S(346, 533), S(345, 532), S(351, 527), S(348, 535), S(347, 537), S(346, 504), S(353, 483), S(378, 470),
        S(343, 531), S(340, 528), S(344, 528), S(350, 537), S(347, 516), S(323, 517), S(344, 504), S(352, 502)
    },

    {
        S(36, -97), S(24, -44), S(71, -36), S(-59, 11), S(-19, -4), S(-5, -1), S(23, -11), S(125, -115),
        S(-92, 0), S(11, 27), S(-19, 40), S(91, 23), S(34, 42), S(29, 53), S(38, 43), S(-44, 14),
        S(-91, 12), S(52, 33), S(11, 58), S(0, 70), S(31, 70), S(82, 61), S(34, 52), S(-28, 18),
        S(-55, 0), S(-26, 37), S(-33, 65), S(-61, 87), S(-57, 87), S(-36, 70), S(-51, 51), S(-118, 25),
        S(-63, -8), S(-30, 25), S(-34, 54), S(-60, 77), S(-55, 76), S(-30, 52), S(-56, 34), S(-127, 19),
        S(-38, -13), S(10, 10), S(-15, 34), S(-18, 45), S(-12, 43), S(-15, 32), S(0, 10), S(-56, 0),
        S(18, -29), S(16, -2), S(15, 10), S(-11, 21), S(-11, 22), S(0, 12), S(28, -6), S(4, -24),
        S(-3, -60), S(42, -43), S(26, -20), S(-43, -3), S(7, -22), S(-27, -9), S(20, -33), S(-5, -59)
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
        S(0, 0), S(0, 0), S(117, 195), S(136, 184), S(154, 217), S(0, 0), S(190, 232), S(0, 0), S(165, 202), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(113, 126), S(124, 128), S(130, 159), S(143, 168), S(148, 177), S(159, 192), S(166, 198), S(170, 209), S(171, 214), S(173, 221), S(174, 218), S(172, 220), S(193, 211), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(178, 251), S(184, 264), S(190, 272), S(192, 276), S(192, 280), S(194, 284), S(199, 287), S(203, 290), S(208, 295), S(213, 297), S(215, 302), S(221, 306), S(222, 309), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(407, 364), S(397, 431), S(422, 477), S(425, 495), S(423, 554), S(428, 564), S(426, 582), S(430, 586), S(431, 597), S(432, 607), S(434, 609), S(437, 613), S(436, 620), S(436, 625), S(437, 631), S(436, 635), S(437, 637), S(438, 644), S(441, 641), S(443, 639), S(452, 632), S(453, 630), S(467, 622), S(502, 602), S(554, 574),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(57, -18), S(63, -18), S(45, 1), S(33, 3), S(27, -1), S(21, 0), S(15, 0), S(14, -1), S(2, 4), S(0, 0), S(-8, 4), S(-18, 5), S(-31, 7), S(-45, 8), S(-60, 7), S(-74, 7), S(-90, 5), S(-92, 0), S(-96, -4), S(-102, -8), S(-112, -14), S(-129, -18), S(-115, -33), S(-116, -37), S(-106, -50),
    },

};

// Bishop pair evaluation
const int32_t bishop_pair = S(20, 61);

// Passed pawn bonus
const int32_t passed_pawns[9] = {
    S(15, -20), S(8, 5), S(-5, 34), S(-19, 47), S(-27, 30), S(-30, 34), S(-80, 32), S(28, -40), S(-121, -49),
};

// King Zone Attacks
// Bonus for pieces (not pawns or kings) which attack squares near the king
const int32_t inner_king_zone_attacks[4]{
    S(11, -6), S(18, -4), S(24, -7), S(14, 7),
};
// Attacking the ring 1 square away from the king square 
const int32_t outer_king_zone_attacks[4]{
    S(0, 1), S(0, 0), S(4, -3), S(2, 1),
};

// Rooks on open file bonus
const int32_t rook_open_file[2] = {
     S(10, 140), S(57, 244),
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
    int32_t num_w_rooks_on_op_file = 0;
    int32_t num_b_rooks_on_op_file = 0;

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
                        if ((is_white ? (WHITE_AHEAD_MASK[sq] & wp & bp) : (BLACK_AHEAD_MASK[sq] & wp & bp)) == 0ull){
                            if (is_white) num_w_rooks_on_op_file++;
                            else num_b_rooks_on_op_file++;
                        }

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

    // Rooks on open files
    if (num_w_rooks_on_op_file == 1) score_array[0] += rook_open_file[0];
    if (num_w_rooks_on_op_file == 2) score_array[0] += rook_open_file[1];
    if (num_b_rooks_on_op_file == 1) score_array[1] += rook_open_file[0];
    if (num_b_rooks_on_op_file == 2) score_array[1] += rook_open_file[1];

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
