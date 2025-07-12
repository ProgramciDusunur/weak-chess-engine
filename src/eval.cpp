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
        S(136, 271), S(168, 261), S(146, 263), S(166, 214), S(157, 212), S(147, 226), S(86, 268), S(42, 279),
        S(66, 209), S(84, 219), S(114, 187), S(116, 163), S(126, 153), S(147, 150), S(130, 191), S(79, 185),
        S(53, 140), S(77, 133), S(79, 117), S(85, 103), S(101, 98), S(97, 107), S(96, 122), S(71, 118),
        S(43, 115), S(70, 116), S(73, 101), S(84, 93), S(87, 93), S(84, 99), S(82, 108), S(59, 98),
        S(45, 108), S(66, 114), S(70, 101), S(74, 104), S(84, 102), S(80, 102), S(96, 104), S(67, 94),
        S(46, 111), S(68, 117), S(63, 107), S(63, 113), S(76, 116), S(83, 107), S(99, 104), S(60, 95),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    },
    {
        S(16, 82), S(18, 143), S(54, 124), S(84, 116), S(120, 120), S(70, 95), S(49, 144), S(61, 59),
        S(116, 138), S(112, 124), S(100, 117), S(111, 119), S(91, 113), S(154, 98), S(119, 119), S(151, 122),
        S(111, 115), S(110, 116), S(154, 166), S(155, 171), S(186, 159), S(194, 151), S(122, 112), S(128, 108),
        S(112, 125), S(89, 130), S(140, 175), S(162, 176), S(129, 183), S(157, 177), S(82, 137), S(137, 120),
        S(101, 125), S(81, 120), S(127, 173), S(126, 173), S(138, 176), S(129, 166), S(95, 122), S(111, 116),
        S(84, 111), S(71, 113), S(114, 151), S(124, 163), S(134, 161), S(120, 145), S(87, 107), S(100, 111),
        S(98, 137), S(86, 114), S(65, 109), S(82, 109), S(80, 107), S(74, 105), S(93, 105), S(122, 145),
        S(77, 127), S(114, 127), S(78, 109), S(90, 111), S(95, 111), S(93, 103), S(112, 136), S(107, 116)
    },
    {
        S(109, 164), S(82, 173), S(84, 167), S(47, 178), S(62, 173), S(78, 164), S(101, 162), S(83, 162),
        S(112, 155), S(128, 154), S(118, 157), S(106, 159), S(120, 150), S(119, 153), S(117, 162), S(113, 154),
        S(125, 171), S(139, 159), S(136, 157), S(140, 150), S(129, 155), S(158, 158), S(143, 159), S(133, 173),
        S(121, 165), S(127, 164), S(130, 157), S(140, 168), S(136, 159), S(131, 162), S(128, 159), S(112, 167),
        S(123, 160), S(113, 164), S(118, 163), S(137, 160), S(133, 159), S(124, 157), S(118, 160), S(133, 152),
        S(127, 163), S(127, 161), S(128, 158), S(126, 157), S(128, 160), S(130, 156), S(132, 153), S(143, 155),
        S(137, 167), S(135, 151), S(137, 143), S(122, 155), S(130, 153), S(132, 149), S(145, 157), S(140, 150),
        S(131, 155), S(142, 163), S(133, 154), S(120, 158), S(126, 157), S(120, 166), S(134, 151), S(146, 141)
    },
    {
        S(184, 297), S(166, 305), S(162, 316), S(158, 313), S(168, 308), S(192, 300), S(191, 299), S(206, 294),
        S(167, 300), S(159, 314), S(173, 318), S(187, 310), S(166, 315), S(194, 303), S(189, 297), S(217, 282),
        S(159, 299), S(179, 300), S(173, 302), S(174, 300), S(198, 291), S(193, 288), S(230, 280), S(201, 277),
        S(153, 301), S(162, 297), S(161, 305), S(167, 300), S(162, 292), S(170, 285), S(171, 285), S(172, 281),
        S(138, 294), S(137, 296), S(145, 298), S(154, 294), S(150, 294), S(138, 291), S(152, 283), S(146, 280),
        S(132, 292), S(137, 289), S(144, 288), S(146, 290), S(146, 287), S(144, 280), S(164, 266), S(150, 269),
        S(134, 285), S(142, 286), S(156, 285), S(156, 285), S(157, 280), S(148, 278), S(161, 270), S(142, 276),
        S(150, 289), S(152, 288), S(160, 292), S(162, 287), S(161, 284), S(145, 287), S(159, 281), S(148, 279)
    },
    {
        S(334, 547), S(320, 563), S(333, 582), S(361, 563), S(349, 570), S(347, 574), S(397, 519), S(359, 546),
        S(348, 540), S(325, 566), S(322, 597), S(313, 611), S(301, 631), S(343, 584), S(345, 570), S(388, 559),
        S(354, 546), S(346, 554), S(345, 583), S(342, 589), S(349, 593), S(370, 576), S(383, 548), S(374, 549),
        S(337, 559), S(342, 565), S(335, 574), S(330, 591), S(330, 593), S(340, 581), S(350, 578), S(347, 567),
        S(345, 551), S(333, 568), S(335, 567), S(340, 577), S(335, 576), S(336, 564), S(342, 558), S(350, 556),
        S(340, 538), S(347, 544), S(341, 555), S(337, 550), S(339, 555), S(343, 547), S(352, 531), S(350, 537),
        S(350, 532), S(348, 531), S(355, 526), S(352, 535), S(351, 536), S(350, 503), S(357, 482), S(381, 469),
        S(346, 532), S(344, 529), S(349, 528), S(353, 537), S(351, 516), S(326, 518), S(348, 503), S(355, 503)
    },
    {
        S(37, -97), S(25, -44), S(71, -35), S(-62, 12), S(-21, -3), S(-6, 0), S(22, -11), S(120, -114),
        S(-86, 0), S(12, 27), S(-16, 39), S(93, 23), S(36, 43), S(34, 52), S(38, 43), S(-41, 14),
        S(-88, 11), S(55, 32), S(12, 58), S(2, 69), S(35, 69), S(86, 60), S(37, 52), S(-28, 19),
        S(-50, 0), S(-24, 37), S(-30, 64), S(-58, 86), S(-54, 85), S(-34, 69), S(-50, 51), S(-117, 25),
        S(-61, -8), S(-26, 24), S(-32, 53), S(-57, 76), S(-55, 74), S(-30, 52), S(-56, 33), S(-127, 19),
        S(-39, -14), S(11, 9), S(-14, 33), S(-18, 45), S(-14, 43), S(-15, 32), S(-1, 10), S(-57, 0),
        S(18, -29), S(16, -2), S(14, 10), S(-11, 20), S(-11, 22), S(0, 12), S(26, -6), S(4, -22),
        S(-3, -60), S(42, -43), S(26, -21), S(-42, -4), S(6, -21), S(-27, -9), S(20, -33), S(-5, -57)
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
        S(0, 0), S(0, 0), S(113, 189), S(136, 185), S(159, 221), S(0, 0), S(195, 235), S(0, 0), S(167, 204), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },
    {
        S(0, 0), S(117, 128), S(127, 130), S(133, 161), S(146, 171), S(151, 179), S(162, 194), S(169, 200), S(173, 211), S(174, 215), S(176, 222), S(177, 219), S(175, 220), S(197, 212), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },
    {
        S(0, 0), S(0, 0), S(189, 303), S(195, 317), S(201, 325), S(204, 330), S(204, 335), S(206, 340), S(211, 343), S(216, 346), S(221, 351), S(226, 354), S(228, 359), S(233, 363), S(232, 366), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },
    {
        S(0, 0), S(0, 0), S(0, 0), S(421, 365), S(410, 433), S(435, 479), S(437, 496), S(434, 555), S(439, 564), S(438, 584), S(441, 588), S(442, 599), S(444, 609), S(446, 611), S(448, 615), S(448, 622), S(447, 627), S(448, 633), S(448, 637), S(449, 639), S(450, 646), S(452, 643), S(454, 641), S(463, 634), S(464, 632), S(479, 624), S(514, 603), S(567, 575),
    },
    {
        S(0, 0), S(0, 0), S(0, 0), S(57, -21), S(62, -22), S(44, -2), S(33, 0), S(27, -3), S(21, 0), S(15, -1), S(14, -2), S(3, 4), S(0, 1), S(-7, 4), S(-18, 6), S(-30, 8), S(-45, 9), S(-60, 8), S(-74, 8), S(-88, 6), S(-90, 1), S(-95, -2), S(-101, -7), S(-112, -13), S(-128, -17), S(-114, -31), S(-115, -35), S(-107, -48),
    },
};


// Bishop pair evaluation
const int32_t bishop_pair = S(19, 63);

// Passed pawn bonus
const int32_t passed_pawns[9] = {
    S(15, -16), S(8, 4), S(-4, 31), S(-17, 43), S(-26, 25), S(-28, 23), S(-69, 12), S(20, -53), S(-131, -53),
};

// King Zone Attacks
// Bonus for pieces (not pawns or kings) which attack squares near the king
const int32_t inner_king_zone_attacks[4] = {
    S(12, -6), S(18, -5), S(24, -7), S(14, 7),
};
// Attacking the ring 1 square away from the king square 
const int32_t outer_king_zone_attacks[4] = {
    S(0, 1), S(0, 0), S(3, -3), S(2, 1),
};

// Doubled pawn penalty
const int32_t doubled_pawn_penalty[8] = {
    S(-11, -45), S(-14, -32), S(-15, -34), S(-14, -17), S(-15, -28), S(-17, -32), S(-16, -29), S(-20, -40),
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
                // Note that for pawns to be considered "doubled", they need not be directly in front
                // of another pawn
                uint64_t front_mask = is_white ? WHITE_AHEAD_MASK[sq] : BLACK_AHEAD_MASK[sq];
                uint64_t our_pawn_bb = is_white ? wp.getBits() : bp.getBits();
                // Doubled pawns
                if (front_mask & our_pawn_bb)
                    score_array[is_white ? 0 : 1] += doubled_pawn_penalty[is_white ? 7 - sq % 8 : sq % 8];

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
