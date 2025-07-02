#include <stdint.h>
#include <chrono> 

#include "chess.hpp"
#include "timeman.hpp"
#include "search.hpp"
#include "eval.hpp"

using namespace chess;

chess::Move root_best_move;

// Search Function
// We are basically using a fail soft "negamax" search, see here for more info: https://minuskelvin.net/chesswiki/content/minimax.html#negamax
// Negamax is basically a simplification of the famed minimax algorithm. Basically, it works by negating the score in the next
// ply. This works because a position which is a win for white is a loss for black and vice versa. Most "strong" chess engines use
// negamax instead of minimax because it makes the code much tidier. Not sure about how much is gains though. The "fail soft" basically
// means we return max_value instead of alpha. This gives us more information to do puning etc etc.
int32_t alpha_beta(Board &board, int32_t depth, int32_t alpha, int32_t beta, int32_t ply){

    // Search variables
    // max_score for fail-soft negamax
    int32_t best_score = -POSITIVE_INFINITY;
    bool is_root = ply == 0;

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


    // Get all legal moves for our moveloop in our search
    Movelist all_moves;
    movegen::legalmoves(all_moves, board);

    // Checkmate detection
    // When we are in checkmate during our turn, we lost the game, therefore we 
    // should return a large negative value
    if (board.inCheck() && all_moves.size() == 0)
        return -POSITIVE_MATE_SCORE;

    // Depth 0 -- we end our search and return eval (haven't started qs yet)
    if (depth == 0)
        return evaluate(board);

    // For updating Transposition table later
    bool alpha_raised = false;    

    // Main move loop
    // For loop is faster tha foreach :)
    for (int idx = 0; idx < all_moves.size(); idx++){
        Move current_move = all_moves[idx];

        // Basic make and undo functionality. Copy-make should be faster but that
        // debugging is for later
        board.makeMove(current_move);
        int32_t score = -alpha_beta(board, depth - 1, -beta, -alpha, ply + 1);
        board.unmakeMove(current_move);

        // Updating best_score and alpha beta pruning
        // I did not actually test this in sprt 
        if (score > best_score){
            best_score = score;

            if (is_root)
                root_best_move = current_move;

            // Update alpha
            if (score > alpha){
                alpha_raised = true;
                alpha = score;

                // Alpha-Beta Pruning
                if (score >= beta){
                    break;
                }
            }
        }
    }

    return best_score;

}

int32_t search_manager(Board &board){
    return 0;
}