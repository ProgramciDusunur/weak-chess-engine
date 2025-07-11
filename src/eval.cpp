#include <stdint.h>

#include "chess.hpp"
#include "packing.hpp"
#include "eval.hpp"
#include "defaults.hpp"

using namespace chess;
using namespace std;

const int32_t PSQT[6][64] = {
    {
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
        S(118, 286), S(147, 277), S(123, 279), S(147, 229), S(135, 225), S(119, 238), S(54, 282), S(29, 295),
        S(53, 224), S(69, 233), S(97, 201), S(100, 180), S(105, 170), S(125, 157), S(110, 202), S(68, 198),
        S(43, 154), S(68, 144), S(67, 126), S(72, 116), S(90, 108), S(80, 111), S(88, 129), S(65, 129),
        S(36, 129), S(62, 127), S(64, 109), S(78, 107), S(81, 104), S(75, 105), S(76, 118), S(55, 110),
        S(39, 123), S(59, 125), S(61, 108), S(67, 118), S(80, 112), S(71, 109), S(92, 115), S(65, 106),
        S(42, 126), S(62, 128), S(56, 115), S(61, 125), S(71, 127), S(85, 114), S(103, 114), S(58, 106),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    },

    {
        S(11, 62), S(16, 139), S(50, 117), S(82, 109), S(116, 111), S(62, 87), S(41, 145), S(57, 43),
        S(116, 132), S(110, 115), S(102, 122), S(119, 121), S(103, 115), S(161, 101), S(111, 110), S(157, 116),
        S(108, 110), S(110, 124), S(149, 164), S(157, 165), S(193, 150), S(197, 144), S(135, 114), S(138, 100),
        S(109, 120), S(91, 139), S(135, 175), S(157, 175), S(137, 177), S(161, 173), S(103, 138), S(143, 112),
        S(96, 121), S(81, 129), S(121, 175), S(123, 175), S(133, 178), S(127, 168), S(100, 131), S(108, 112),
        S(80, 105), S(71, 123), S(110, 153), S(119, 168), S(131, 165), S(117, 149), S(92, 118), S(97, 107),
        S(97, 134), S(83, 111), S(67, 120), S(87, 121), S(85, 122), S(87, 116), S(100, 103), S(122, 143),
        S(61, 113), S(116, 125), S(78, 104), S(93, 108), S(95, 110), S(102, 100), S(118, 132), S(92, 101)
    },

    {
        S(125, 169), S(93, 177), S(100, 172), S(62, 184), S(81, 177), S(95, 167), S(119, 163), S(100, 167),
        S(129, 158), S(140, 160), S(132, 161), S(116, 162), S(144, 153), S(141, 153), S(141, 163), S(139, 157),
        S(138, 180), S(153, 164), S(149, 162), S(164, 152), S(153, 157), S(190, 159), S(173, 160), S(171, 173),
        S(136, 173), S(139, 170), S(150, 161), S(161, 171), S(157, 165), S(152, 165), S(142, 167), S(136, 173),
        S(136, 167), S(133, 169), S(132, 169), S(154, 165), S(150, 165), S(135, 164), S(131, 167), S(149, 158),
        S(145, 170), S(143, 167), S(143, 165), S(140, 164), S(143, 167), S(144, 164), S(147, 159), S(158, 161),
        S(155, 170), S(151, 158), S(152, 150), S(137, 164), S(144, 162), S(155, 155), S(167, 164), S(155, 155),
        S(147, 163), S(160, 168), S(152, 161), S(137, 167), S(146, 166), S(142, 173), S(161, 154), S(158, 152)
    },

    {
        S(194, 298), S(176, 307), S(176, 316), S(176, 311), S(193, 304), S(212, 297), S(214, 295), S(232, 290),
        S(174, 301), S(169, 314), S(186, 317), S(204, 307), S(192, 308), S(218, 295), S(215, 290), S(246, 277),
        S(156, 302), S(176, 304), S(170, 305), S(173, 303), S(201, 290), S(206, 285), S(253, 276), S(232, 271),
        S(150, 303), S(157, 301), S(156, 309), S(161, 305), S(165, 291), S(173, 286), S(185, 284), S(190, 279),
        S(138, 296), S(135, 299), S(144, 300), S(151, 298), S(153, 294), S(146, 292), S(166, 282), S(159, 279),
        S(136, 292), S(137, 291), S(146, 289), S(150, 291), S(158, 286), S(158, 278), S(184, 262), S(166, 265),
        S(137, 287), S(143, 288), S(158, 288), S(159, 289), S(163, 281), S(167, 276), S(179, 269), S(154, 275),
        S(153, 287), S(156, 288), S(165, 292), S(171, 288), S(176, 281), S(169, 284), S(178, 276), S(158, 275)
    },

    {
        S(368, 560), S(347, 584), S(366, 601), S(395, 587), S(399, 586), S(405, 585), S(445, 531), S(400, 565),
        S(375, 550), S(349, 578), S(351, 609), S(343, 625), S(348, 643), S(384, 605), S(375, 590), S(424, 582),
        S(376, 555), S(367, 562), S(362, 594), S(371, 600), S(381, 611), S(423, 593), S(433, 564), S(435, 569),
        S(360, 563), S(360, 570), S(359, 580), S(358, 596), S(358, 609), S(373, 602), S(381, 598), S(389, 593),
        S(365, 557), S(356, 576), S(356, 574), S(364, 587), S(364, 585), S(364, 583), S(372, 580), S(382, 578),
        S(365, 548), S(368, 554), S(366, 563), S(366, 559), S(369, 563), S(374, 563), S(384, 553), S(383, 552),
        S(375, 541), S(370, 540), S(379, 533), S(383, 536), S(379, 543), S(389, 518), S(394, 498), S(414, 481),
        S(371, 538), S(372, 534), S(379, 531), S(386, 545), S(384, 528), S(369, 533), S(391, 512), S(388, 510)
    },

};

const int32_t mobilities[4][28]{
    {
        S(0, 0), S(0, 0), S(120, 199), S(132, 184), S(158, 221), S(0, 0), S(190, 222), S(0, 0), S(169, 198), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(113, 111), S(125, 123), S(132, 155), S(145, 164), S(150, 173), S(162, 188), S(170, 194), S(176, 205), S(178, 209), S(183, 215), S(185, 211), S(186, 212), S(212, 202), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(176, 294), S(184, 307), S(191, 314), S(196, 317), S(198, 320), S(201, 326), S(207, 328), S(214, 330), S(221, 335), S(227, 337), S(231, 341), S(240, 344), S(244, 344), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(427, 191), S(372, 397), S(399, 445), S(399, 475), S(398, 532), S(403, 546), S(404, 563), S(408, 569), S(411, 579), S(413, 590), S(416, 593), S(419, 599), S(420, 607), S(421, 614), S(423, 621), S(423, 628), S(424, 632), S(426, 641), S(430, 641), S(431, 644), S(440, 641), S(442, 639), S(455, 638), S(499, 614), S(562, 588),
    },

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

            // Mobilities for knight - queen
            if (j > 0 && j < 5){
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

                }
                score_array[is_white ? 0 : 1] += mobilities[j-1][attacks];
            }
        }

    }

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
