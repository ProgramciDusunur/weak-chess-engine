#include <stdint.h>
#include <chrono> 

#include "chess.hpp"
#include "timeman.hpp"
#include "search.hpp"

using namespace chess;

// Search Function
// We are basically using a fail soft "negamax" search, see here for more info: https://minuskelvin.net/chesswiki/content/minimax.html#negamax
// Negamax is basically a simplification of the famed minimax algorithm. Basically, it works by negating the score in the next
// ply. This works because a position which is a win for white is a loss for black and vice versa. Most "strong" chess engines use
// negamax instead of minimax because it makes the code much tidier. Not sure about how much is gains though. The "fail soft" basically
// means we return max_value instead of alpha. This gives us more information to do puning etc etc.
int32_t alpha_beta(Board &board){

    // Handle time management
    // Here is where our hard-bound time mnagement is. When the search time 
    // exceeds our maximum hard bound time limit
    if (hard_bound_time_exceeded())
        throw SearchAbort();

    // Draw detections
    // Ensure all drawn positions have a score of 0. This is important so
    // our engine will not mistake a drawn position with a winning one
    // Say we are up a bishop in the endgame. Although static eval may tell
    // us that we are +bishop_value, the actual eval should be 0. Some 
    // engines have a "contempt" variable in draw detections to make engines
    // avoid draws more or like drawn positions. This surely weakens the
    // engine when playing against another at the same level. But it is
    // irrelevant in our case.
    if (board.isHalfMoveDraw() || board.isInsufficientMaterial() || board.isRepetition())
        return 0;


}

int32_t search_manager(Board &board){

}