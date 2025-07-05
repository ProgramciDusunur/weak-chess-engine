#pragma once

#include <algorithm>
#include <vector>

#include "chess.hpp"
#include "ordering.hpp"
#include "transposition.hpp"
#include "mvv_lva.hpp"

// High bonus score for TT move
constexpr int32_t TT_BONUS = 1000000;
constexpr int32_t KILLER_BONUS = 90000;

// Sorts moves by giving TT move highest priority, then by MVV-LVA score for captures
inline void sort_moves(chess::Board& board, chess::Movelist& movelist, bool tt_hit, uint16_t tt_move, int32_t ply) {
    // Pair each move with its score
    std::vector<std::pair<chess::Move, int32_t>> scored_moves;

    for (const auto& move : movelist) {
        int32_t score = 0;

        // TT move bonus
        if (tt_hit && encode_move(move.from(), move.to(), move.typeOf()) == tt_move) {
            score += TT_BONUS;
        }

        // Don't waste time on mvv-lva when it is already a tt move
        else if (board.isCapture(move)) {
            // MVV-LVA score (captures only)
            score += mvv_lva(board, move);
        }
        else {
            // Killer moves
            if (killers[0][ply] == move || killers[1][ply] == move){
                score += KILLER_BONUS;
            }
            else {
                score += quiet_history[board.sideToMove() == chess::Color::WHITE][move.from().index()][move.to().index()];
            }
        }

        scored_moves.emplace_back(move, score);
    }

    // Sort in descending order of score
    std::sort(scored_moves.begin(), scored_moves.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Update movelist with sorted moves
    for (size_t i = 0; i < movelist.size(); ++i) {
        movelist[i] = scored_moves[i].first;
    }
}

inline void sort_captures(chess::Board& board, chess::Movelist& movelist, bool tt_hit, uint16_t tt_move) {
    // Pair each move with its score
    std::vector<std::pair<chess::Move, int32_t>> scored_moves;

    for (const auto& move : movelist) {
        int32_t score = 0;

        // TT move bonus
        if (tt_hit && encode_move(move.from(), move.to(), move.typeOf()) == tt_move) {
            score += TT_BONUS;
        }

        // Don't waste time on mvv-lva when it is already a tt move
        else {
            // MVV-LVA score (captures only)
            score += mvv_lva(board, move);
        }

        scored_moves.emplace_back(move, score);
    }

    // Sort in descending order of score
    std::sort(scored_moves.begin(), scored_moves.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Update movelist with sorted moves
    for (size_t i = 0; i < movelist.size(); ++i) {
        movelist[i] = scored_moves[i].first;
    }
}
