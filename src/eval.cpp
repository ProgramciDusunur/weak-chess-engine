#include <stdint.h>

#include "chess.hpp"
#include "packing.hpp"
#include "eval.hpp"
#include "defaults.hpp"

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
        S(119, 287), S(149, 278), S(126, 281), S(149, 234), S(141, 229), S(123, 243), S(65, 282), S(29, 296),
        S(53, 225), S(70, 234), S(97, 202), S(100, 182), S(109, 172), S(126, 159), S(112, 202), S(66, 199),
        S(44, 154), S(68, 144), S(68, 126), S(74, 116), S(91, 107), S(82, 111), S(86, 129), S(63, 130),
        S(36, 129), S(62, 127), S(64, 109), S(77, 106), S(80, 103), S(74, 103), S(74, 117), S(53, 110),
        S(38, 123), S(58, 125), S(60, 107), S(67, 116), S(77, 110), S(69, 106), S(88, 114), S(62, 106),
        S(41, 126), S(62, 128), S(55, 113), S(57, 123), S(68, 125), S(73, 113), S(92, 114), S(54, 107),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    },

    {
        S(11, 69), S(22, 139), S(60, 118), S(96, 109), S(131, 112), S(83, 87), S(51, 143), S(63, 44),
        S(120, 132), S(115, 118), S(114, 126), S(129, 125), S(112, 119), S(180, 103), S(124, 112), S(162, 114),
        S(113, 112), S(120, 127), S(154, 166), S(162, 168), S(202, 153), S(204, 147), S(149, 117), S(142, 101),
        S(114, 123), S(101, 143), S(138, 176), S(162, 178), S(141, 180), S(165, 176), S(112, 142), S(147, 115),
        S(101, 125), S(90, 133), S(124, 176), S(126, 177), S(136, 179), S(130, 169), S(108, 133), S(113, 115),
        S(85, 109), S(80, 127), S(112, 156), S(121, 168), S(131, 165), S(119, 150), S(99, 121), S(101, 110),
        S(99, 135), S(87, 114), S(76, 123), S(93, 124), S(91, 124), S(84, 120), S(97, 104), S(124, 144),
        S(64, 117), S(116, 127), S(79, 108), S(92, 112), S(97, 111), S(95, 102), S(116, 135), S(95, 102)
    },

    {
        S(114, 167), S(90, 172), S(94, 168), S(60, 178), S(84, 172), S(100, 162), S(115, 161), S(97, 161),
        S(122, 153), S(132, 156), S(125, 158), S(111, 160), S(139, 149), S(139, 151), S(141, 159), S(135, 152),
        S(130, 175), S(146, 161), S(140, 160), S(155, 149), S(148, 154), S(183, 156), S(167, 158), S(162, 169),
        S(128, 168), S(131, 167), S(142, 158), S(154, 169), S(148, 161), S(143, 164), S(133, 162), S(128, 167),
        S(128, 162), S(124, 166), S(125, 165), S(144, 161), S(140, 162), S(126, 160), S(122, 163), S(139, 154),
        S(136, 165), S(135, 163), S(133, 160), S(131, 158), S(131, 161), S(134, 159), S(137, 154), S(149, 157),
        S(147, 167), S(142, 153), S(143, 144), S(125, 157), S(132, 155), S(135, 151), S(150, 159), S(146, 151),
        S(138, 158), S(149, 164), S(139, 154), S(123, 160), S(131, 159), S(123, 168), S(142, 151), S(149, 144)
    },

    {
        S(200, 300), S(183, 308), S(185, 316), S(188, 310), S(205, 302), S(226, 296), S(219, 297), S(232, 293),
        S(177, 304), S(172, 315), S(190, 319), S(209, 309), S(197, 309), S(229, 296), S(222, 292), S(246, 279),
        S(158, 304), S(179, 305), S(174, 306), S(178, 304), S(209, 291), S(211, 286), S(256, 277), S(229, 273),
        S(152, 305), S(160, 302), S(159, 309), S(166, 304), S(168, 292), S(179, 286), S(187, 284), S(189, 280),
        S(138, 298), S(137, 300), S(146, 300), S(155, 297), S(156, 293), S(150, 290), S(166, 282), S(159, 280),
        S(136, 294), S(139, 291), S(148, 289), S(153, 289), S(157, 284), S(159, 276), S(183, 261), S(165, 266),
        S(136, 289), S(144, 289), S(159, 288), S(159, 288), S(163, 280), S(158, 277), S(173, 270), S(151, 277),
        S(153, 290), S(155, 290), S(163, 293), S(165, 290), S(168, 282), S(153, 286), S(168, 280), S(155, 280)
    },

    {
        S(373, 563), S(355, 585), S(375, 602), S(404, 588), S(404, 591), S(409, 589), S(449, 535), S(407, 565),
        S(379, 554), S(353, 582), S(356, 613), S(347, 630), S(352, 648), S(392, 609), S(389, 586), S(430, 580),
        S(379, 559), S(371, 565), S(366, 596), S(375, 603), S(389, 612), S(424, 597), S(437, 564), S(438, 569),
        S(364, 566), S(364, 572), S(363, 583), S(364, 597), S(363, 612), S(377, 604), S(385, 599), S(392, 593),
        S(368, 561), S(359, 581), S(361, 577), S(366, 591), S(367, 587), S(368, 584), S(376, 582), S(386, 581),
        S(368, 551), S(371, 559), S(368, 568), S(368, 560), S(369, 565), S(376, 563), S(386, 552), S(385, 557),
        S(377, 546), S(373, 543), S(381, 536), S(382, 540), S(380, 545), S(382, 520), S(388, 501), S(412, 489),
        S(371, 546), S(371, 540), S(376, 536), S(383, 545), S(381, 523), S(357, 535), S(379, 519), S(384, 518)
    },

    {
        S(29, -89), S(15, -38), S(62, -31), S(-82, 19), S(-30, 0), S(-12, 2), S(21, -7), S(103, -107),
        S(-102, 5), S(-19, 35), S(-48, 49), S(57, 32), S(3, 51), S(-2, 61), S(6, 51), S(-52, 19),
        S(-108, 19), S(21, 42), S(-17, 69), S(-31, 81), S(6, 80), S(57, 71), S(10, 61), S(-49, 26),
        S(-73, 8), S(-56, 47), S(-56, 74), S(-87, 98), S(-81, 97), S(-58, 80), S(-80, 61), S(-135, 33),
        S(-75, -1), S(-55, 33), S(-63, 64), S(-86, 87), S(-86, 86), S(-60, 62), S(-84, 43), S(-137, 23),
        S(-46, -9), S(-9, 17), S(-42, 43), S(-48, 55), S(-45, 54), S(-46, 42), S(-25, 18), S(-62, 3),
        S(22, -29), S(7, 0), S(2, 14), S(-26, 26), S(-28, 28), S(-14, 17), S(20, -4), S(9, -23),
        S(4, -60), S(46, -43), S(30, -20), S(-43, -2), S(11, -23), S(-27, -8), S(30, -36), S(10, -61)
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
const int32_t mobilities[5][28]{
    {
        S(0, 0), S(0, 0), S(120, 198), S(131, 184), S(155, 220), S(0, 0), S(182, 220), S(0, 0), S(167, 198), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(105, 119), S(118, 123), S(124, 154), S(137, 164), S(143, 172), S(154, 188), S(162, 193), S(169, 204), S(170, 207), S(175, 213), S(178, 209), S(180, 208), S(206, 198), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(180, 297), S(187, 308), S(194, 316), S(198, 319), S(199, 323), S(201, 328), S(207, 331), S(213, 333), S(220, 338), S(226, 340), S(231, 343), S(239, 346), S(243, 347), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(410, 266), S(372, 403), S(397, 453), S(401, 471), S(399, 533), S(405, 546), S(404, 566), S(408, 572), S(410, 584), S(412, 595), S(415, 598), S(418, 603), S(419, 611), S(419, 618), S(421, 625), S(422, 631), S(423, 635), S(425, 643), S(429, 642), S(431, 644), S(440, 641), S(440, 640), S(453, 638), S(492, 616), S(544, 596),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(56, -14), S(64, -17), S(45, 1), S(35, 2), S(30, -1), S(23, 0), S(17, 0), S(16, -1), S(4, 5), S(1, 1), S(-7, 5), S(-19, 7), S(-33, 10), S(-49, 11), S(-65, 10), S(-80, 11), S(-96, 9), S(-100, 4), S(-106, 0), S(-113, -4), S(-123, -11), S(-136, -16), S(-123, -30), S(-126, -35), S(-118, -47),
    },
};

// Bishop pair evaluation
const int32_t bishop_pair = S(18, 62);

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
            if (j > 0){
                int32_t attacks = 0;
                switch (j)
                {
                    // knights
                    case 1:
                        attacks = chess::attacks::knight(static_cast<chess::Square>(sq)).count();
                        break;
                    // bishops
                    case 2:
                        attacks = chess::attacks::bishop(static_cast<chess::Square>(sq), board.occ()).count();
                        break;
                    // rooks
                    case 3:
                        attacks = chess::attacks::rook(static_cast<chess::Square>(sq), board.occ()).count();
                        break;
                    // queens
                    case 4:
                        attacks = chess::attacks::queen(static_cast<chess::Square>(sq), board.occ()).count();
                        break;
                    // king virtual mobility
                    case 5:
                        attacks = chess::attacks::queen(static_cast<chess::Square>(sq), board.occ()).count();
                        break;

                }
                score_array[is_white ? 0 : 1] += mobilities[j-1][attacks];
            }
        }

    }

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
