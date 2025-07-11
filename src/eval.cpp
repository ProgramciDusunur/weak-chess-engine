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
        S(117, 287), S(146, 278), S(123, 280), S(147, 231), S(134, 226), S(118, 240), S(54, 283), S(28, 296),
        S(53, 225), S(69, 234), S(97, 201), S(99, 181), S(103, 171), S(125, 158), S(111, 202), S(68, 199),
        S(43, 154), S(68, 144), S(67, 126), S(72, 117), S(90, 108), S(81, 112), S(88, 129), S(65, 130),
        S(36, 130), S(62, 128), S(64, 110), S(78, 108), S(81, 105), S(75, 105), S(76, 118), S(55, 111),
        S(39, 123), S(58, 126), S(61, 108), S(67, 119), S(80, 112), S(71, 109), S(92, 116), S(65, 106),
        S(42, 126), S(62, 129), S(56, 115), S(61, 126), S(71, 127), S(85, 114), S(103, 115), S(58, 107),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    },

    {
        S(15, 68), S(18, 140), S(55, 119), S(89, 111), S(122, 113), S(68, 89), S(43, 145), S(61, 47),
        S(118, 133), S(113, 118), S(111, 126), S(126, 125), S(110, 118), S(169, 103), S(114, 113), S(158, 117),
        S(111, 113), S(118, 127), S(152, 166), S(159, 167), S(195, 151), S(199, 146), S(143, 116), S(141, 101),
        S(112, 124), S(99, 143), S(137, 176), S(160, 177), S(140, 178), S(163, 174), S(111, 141), S(146, 115),
        S(99, 125), S(89, 133), S(123, 177), S(126, 177), S(136, 180), S(129, 170), S(108, 134), S(111, 116),
        S(83, 109), S(79, 127), S(112, 157), S(121, 171), S(134, 168), S(120, 152), S(100, 122), S(99, 110),
        S(98, 136), S(86, 115), S(76, 125), S(95, 126), S(93, 126), S(94, 120), S(103, 106), S(122, 145),
        S(62, 117), S(117, 128), S(81, 109), S(95, 112), S(98, 115), S(105, 104), S(118, 134), S(93, 102)
    },

    {
        S(115, 167), S(84, 173), S(90, 169), S(50, 180), S(71, 174), S(85, 164), S(107, 162), S(91, 162),
        S(118, 153), S(129, 156), S(121, 158), S(106, 160), S(134, 150), S(130, 151), S(130, 159), S(129, 153),
        S(128, 175), S(143, 161), S(138, 160), S(153, 149), S(142, 154), S(179, 155), S(163, 157), S(160, 168),
        S(126, 168), S(128, 167), S(140, 158), S(150, 170), S(146, 161), S(142, 163), S(132, 162), S(126, 168),
        S(126, 163), S(122, 166), S(122, 166), S(143, 162), S(140, 163), S(124, 161), S(121, 163), S(138, 154),
        S(135, 165), S(133, 163), S(133, 161), S(130, 160), S(133, 164), S(134, 161), S(136, 156), S(148, 157),
        S(145, 166), S(141, 154), S(142, 146), S(126, 159), S(133, 157), S(145, 151), S(156, 161), S(145, 151),
        S(136, 158), S(150, 164), S(141, 156), S(127, 161), S(135, 161), S(131, 168), S(150, 150), S(147, 146)
    },

    {
        S(194, 300), S(175, 309), S(174, 318), S(176, 313), S(192, 306), S(213, 299), S(212, 297), S(232, 292),
        S(174, 303), S(169, 315), S(186, 319), S(203, 309), S(191, 309), S(217, 297), S(215, 292), S(245, 278),
        S(155, 304), S(176, 305), S(170, 307), S(172, 305), S(200, 292), S(205, 286), S(252, 278), S(232, 273),
        S(149, 306), S(158, 302), S(155, 310), S(160, 306), S(164, 293), S(174, 287), S(186, 285), S(190, 281),
        S(138, 298), S(135, 300), S(143, 301), S(151, 299), S(154, 296), S(146, 293), S(167, 283), S(159, 280),
        S(136, 294), S(137, 292), S(146, 290), S(149, 292), S(157, 287), S(158, 279), S(184, 263), S(166, 266),
        S(136, 288), S(143, 290), S(158, 289), S(159, 290), S(163, 283), S(167, 277), S(180, 270), S(155, 276),
        S(153, 289), S(156, 290), S(165, 294), S(171, 290), S(176, 283), S(169, 286), S(178, 277), S(158, 277)
    },

    {
        S(370, 560), S(347, 586), S(366, 603), S(394, 589), S(397, 589), S(402, 589), S(444, 533), S(402, 566),
        S(376, 552), S(350, 580), S(351, 611), S(343, 628), S(348, 645), S(385, 607), S(376, 591), S(425, 583),
        S(377, 557), S(368, 564), S(362, 596), S(371, 603), S(381, 614), S(422, 596), S(433, 566), S(437, 570),
        S(361, 565), S(361, 572), S(359, 583), S(359, 597), S(359, 612), S(374, 605), S(382, 600), S(390, 594),
        S(365, 560), S(357, 579), S(358, 575), S(364, 590), S(365, 586), S(365, 585), S(373, 582), S(383, 580),
        S(365, 550), S(369, 557), S(367, 566), S(367, 560), S(370, 565), S(374, 566), S(385, 555), S(384, 553),
        S(376, 542), S(371, 543), S(380, 536), S(384, 539), S(381, 545), S(390, 520), S(395, 500), S(414, 484),
        S(371, 541), S(373, 536), S(380, 534), S(387, 547), S(384, 531), S(369, 537), S(393, 514), S(389, 513)
    },

    {
        S(28, -79), S(11, -30), S(47, -21), S(-98, 27), S(-51, 9), S(-2, 11), S(40, 2), S(150, -103),
        S(-99, 13), S(-47, 40), S(-86, 52), S(15, 34), S(-36, 55), S(-35, 67), S(-5, 57), S(-31, 27),
        S(-115, 28), S(-9, 46), S(-74, 65), S(-92, 75), S(-55, 75), S(14, 67), S(-8, 66), S(-45, 36),
        S(-86, 18), S(-96, 51), S(-111, 70), S(-152, 82), S(-145, 82), S(-109, 76), S(-108, 67), S(-136, 42),
        S(-82, 6), S(-89, 37), S(-119, 60), S(-148, 75), S(-148, 74), S(-111, 61), S(-115, 50), S(-140, 34),
        S(-43, -2), S(-28, 20), S(-81, 41), S(-94, 52), S(-89, 52), S(-86, 43), S(-44, 24), S(-60, 12),
        S(39, -22), S(0, 3), S(-13, 15), S(-44, 25), S(-48, 29), S(-29, 20), S(14, 1), S(22, -15),
        S(29, -56), S(59, -39), S(36, -19), S(-52, -2), S(6, -22), S(-29, -5), S(40, -31), S(37, -60)
    },

};

const int32_t mobilities[4][28]{
    {
        S(0, 0), S(0, 0), S(119, 198), S(132, 184), S(155, 220), S(0, 0), S(182, 220), S(0, 0), S(166, 198), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(105, 115), S(117, 124), S(125, 155), S(137, 163), S(142, 172), S(154, 188), S(162, 193), S(169, 204), S(170, 207), S(175, 214), S(177, 209), S(178, 210), S(204, 200), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(175, 297), S(183, 308), S(190, 316), S(195, 319), S(198, 322), S(201, 328), S(206, 330), S(213, 333), S(220, 337), S(226, 339), S(231, 343), S(239, 346), S(243, 346), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
    },

    {
        S(0, 0), S(0, 0), S(0, 0), S(410, 274), S(368, 411), S(397, 454), S(398, 478), S(398, 535), S(403, 549), S(403, 566), S(407, 572), S(410, 582), S(413, 593), S(415, 596), S(419, 601), S(419, 610), S(420, 616), S(422, 623), S(423, 630), S(424, 634), S(425, 643), S(429, 643), S(430, 645), S(439, 643), S(440, 642), S(452, 641), S(493, 619), S(544, 601),
    },

};

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
