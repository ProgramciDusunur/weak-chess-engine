#pragma once
#include <stdexcept>
#include <stdint.h>

#include "chess.hpp"

// For mate scoring and default value form max_score
constexpr int32_t POSITIVE_MATE_SCORE = 50000;
constexpr int32_t POSITIVE_INFINITY = 100000;
constexpr int32_t DEFAULT_ALPHA = -POSITIVE_INFINITY;
constexpr int32_t DEFAULT_BETA = POSITIVE_INFINITY;

// Maximum search depth
constexpr int32_t MAX_SEARCH_DEPTH = 1023;

// Sarch constants
constexpr int32_t reverse_futility_margin = 60;
constexpr int32_t reverse_futility_depth = 8;
constexpr int32_t null_move_depth = 2;
constexpr int32_t null_move_reduction = 4;
constexpr int32_t late_move_reduction_depth = 3;
constexpr int32_t late_move_reduction_base = 75;
constexpr int32_t late_move_reduction_multiplier = 40;
constexpr int32_t aspiration_window_depth = 4;
constexpr int32_t aspiration_window_delta = 40;
constexpr int32_t maximum_aspiration_window_research = 2;

// Our custom error
struct SearchAbort : public std::exception {
    const char* what() const noexcept override {
        return "Abort Search!!!! Ahhh!!";
    }
};

// The global best move variable
extern chess::Move root_best_move;

// Killers
extern chess::Move killers[2][1024];
inline void reset_killers(){
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 1024; ++j)
            killers[i][j] = chess::Move{};
}

// Quiet History [color][from][to]
constexpr int32_t MAX_HISTORY = 16384;
extern int32_t quiet_history[2][64][64];
inline void reset_quiet_history() {
    for (int color = 0; color < 2; ++color) {
        for (int piece = 0; piece < 64; ++piece) {
            for (int square = 0; square < 64; ++square) {
                quiet_history[color][piece][square] = 0;
            }
        }
    }
}

// The global depth variable
extern int32_t global_depth;
extern int64_t total_nodes;

// Search Function
// We are basically using a fail soft "negamax" search, see here for more info: https://minuskelvin.net/chesswiki/content/minimax.html#negamax
// Negamax is basically a simplification of the famed minimax algorithm. Basically, it works by negating the score in the next
// ply. This works because a position which is a win for white is a loss for black and vice versa. Most "strong" chess engines use
// negamax instead of minimax because it makes the code much tidier. Not sure about how much is gains though. The "fail soft" basically
// means we return max_value instead of alpha. This gives us more information to do puning etc etc.
int32_t alpha_beta(chess::Board &board, int32_t depth, int32_t alpha, int32_t beta, int32_t ply);

// Root of the search function basically
int32_t search_root(chess::Board &board);