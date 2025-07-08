#pragma once

#include <array>
#include <cstdint>
#include <utility>
#include <cassert>

#include "chess.hpp"
#include "ordering.hpp"
#include "transposition.hpp"
#include "mvv_lva.hpp"
#include "see.hpp"

#include <algorithm>

constexpr int32_t TT_BONUS = 1000000;
constexpr int32_t KILLER_BONUS = 90000;

inline void sort_moves(chess::Board& board, chess::Movelist& movelist, bool tt_hit, uint16_t tt_move, int32_t ply) {
    const size_t move_count = movelist.size();

    std::array<std::pair<chess::Move, int32_t>, 256> scored_moves;

    for (size_t i = 0; i < move_count; ++i) {
        const auto& move = movelist[i];
        int32_t score = 0;

        if (tt_hit && move.move() == tt_move) {
            score = TT_BONUS;
        } else if (board.isCapture(move)) {
            score = mvv_lva(board, move);
        } else if (killers[0][ply] == move || killers[1][ply] == move) {
            score = KILLER_BONUS;
        } else {
            score = quiet_history[board.sideToMove() == chess::Color::WHITE][move.from().index()][move.to().index()];
        }

        scored_moves[i] = { move, score };
    }

    std::stable_sort(scored_moves.begin(), scored_moves.begin() + move_count,
        [](const auto& a, const auto& b) { return a.second > b.second; });

    for (size_t i = 0; i < move_count; ++i) {
        movelist[i] = scored_moves[i].first;
    }
}

inline void sort_captures(chess::Board& board, chess::Movelist& movelist, bool tt_hit, uint16_t tt_move) {
    const size_t move_count = movelist.size();

    std::array<std::pair<chess::Move, int32_t>, 256> scored_moves;

    for (size_t i = 0; i < move_count; ++i) {
        const auto& move = movelist[i];
        int32_t score = 0;

        if (tt_hit && move.move() == tt_move) {
            score = TT_BONUS;
        } else {
            score = mvv_lva(board, move);
        } 

        scored_moves[i] = { move, score };
    }

    std::stable_sort(scored_moves.begin(), scored_moves.begin() + move_count,
        [](const auto& a, const auto& b) { return a.second > b.second; });

    for (size_t i = 0; i < move_count; ++i) {
        movelist[i] = scored_moves[i].first;
    }
}
