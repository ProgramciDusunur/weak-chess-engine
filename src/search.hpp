#pragma once
#include <stdexcept>
#include <stdint.h>

#include "chess.hpp"

// For mate scoring and default value form max_score
constexpr int32_t POSITIVE_MATE_SCORE = 50000;
constexpr int32_t POSITIVE_INFINITY = 60000;
constexpr int32_t DEFAULT_ALPHA = -POSITIVE_INFINITY;
constexpr int32_t DEFAULT_BETA = POSITIVE_INFINITY;

// Our custom error
struct SearchAbort : public std::exception {
    const char* what() const noexcept override {
        return "Abort Search!!!! Ahhh!!";
    }
};

// The global best move variable
extern chess::Move root_best_move;

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