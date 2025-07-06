#include <stdint.h>
#include <iostream>
#include <chrono> 
#include <algorithm>
#include <cmath>

#include "chess.hpp"
#include "timeman.hpp"
#include "search.hpp"
#include "eval.hpp"
#include "transposition.hpp"
#include "ordering.hpp"
#include "see.hpp"

using namespace chess;
using namespace std;

chess::Move root_best_move;
chess::Move killers[2][1024]{};
int32_t quiet_history[2][64][64]{};
int32_t global_depth = 0;
int64_t total_nodes = 0;

// Quiescence search. When we are in a noisy position (there are captures), we try to "quiet" the position by
// going down capture trees using negamax and return the eval when we re in a quiet position
int32_t q_search(Board &board, int32_t alpha, int32_t beta, int32_t ply){
    // Increment node count
    total_nodes++;

    // Handle time management
    // Here is also where our hard-bound time mnagement is. When the search time 
    // exceeds our maximum hard bound time limit
    if (global_depth > 1 && hard_bound_time_exceeded())
        throw SearchAbort();

    // Draw detections
    if ((board.isHalfMoveDraw() || board.isInsufficientMaterial() || board.isRepetition()))
        return 0;

    // Get the TT Entry for current position
    TTEntry entry;
    uint64_t zobrists_key = board.zobrist(); 
    bool tt_hit = tt.probe(zobrists_key, entry);

    // Transposition Table cutoffs
    if (tt_hit && ((entry.type == NodeType::EXACT) || (entry.type == NodeType::LOWERBOUND && entry.score >= beta) || (entry.type == NodeType::UPPERBOUND && entry.score <= alpha)))
        return entry.score;
        
    // For TT updating later to determine bound
    int32_t old_alpha = alpha;

    // Eval pruning - If a static evaluation of the board will
    // exceed beta, then we can stop the search here. Also, if the static
    // eval exceeds alpha, we can call our static eval the new alpha (comment from Ethereal)
    int32_t eval = evaluate(board);
    int32_t best_score = eval;
    if (alpha > eval) eval = alpha;
    if (alpha >= beta) return eval;

    // Get all legal moves for our moveloop in our search
    Movelist capture_moves;
    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(capture_moves, board);

    // Move ordering
    sort_captures(board, capture_moves, tt_hit, entry.best_move);

    Move current_best_move;
    for (int idx = 0; idx < capture_moves.size(); idx++){
        Move current_move = capture_moves[idx];

        // QSEE pruning, if a move is obviously losing, don't search it
        if (!see(board, current_move)) continue;

        // Basic make and undo functionality. Copy-make should be faster but that
        // debugging is for later
        board.makeMove(current_move);
        int32_t score = -q_search(board, -beta, -alpha, ply + 1);
        board.unmakeMove(current_move);

        // Updating best_score and alpha beta pruning
        if (score > best_score){
            best_score = score;
            current_best_move = current_move;
        }

        // Update alpha
        if (score > alpha){
            alpha = score;
        }

        // Alpha-Beta Pruning
        if (score >= beta){
            break;
        }
    }

    NodeType bound = best_score >= beta ? NodeType::LOWERBOUND : best_score > old_alpha ? NodeType::EXACT : NodeType::UPPERBOUND;
    uint16_t best_move_tt = bound == NodeType::UPPERBOUND ? 0 : encode_move(current_best_move.from(), current_best_move.to(), current_best_move.typeOf());

    // Storing transpositions
    tt.store(zobrists_key, best_score, 0, bound, best_move_tt);

    return best_score;
}

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
    bool node_is_check = board.inCheck();
    // Important for cutoffs
    bool pv_node = beta - alpha != 1;

    // For updating Transposition table later
    int32_t old_alpha = alpha;  

    // Increment node count
    total_nodes++;

    // Handle time management
    // Here is where our hard-bound time mnagement is. When the search time 
    // exceeds our maximum hard bound time limit
    if (global_depth > 1 && hard_bound_time_exceeded())
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
    if (!is_root && (board.isHalfMoveDraw() || board.isInsufficientMaterial() || board.isRepetition()))
        return 0;

    // Get all legal moves for our moveloop in our search
    Movelist all_moves;
    movegen::legalmoves(all_moves, board);

    // Checkmate detection
    // When we are in checkmate during our turn, we lost the game, therefore we 
    // should return a large negative value
    if (!is_root && all_moves.size() == 0){
        if (node_is_check)
            return -POSITIVE_MATE_SCORE + ply;

        // Stalemate jumpscare!
        else {
            return 0;
        }
    }

    // Depth <= 0 (because we allow depth to drop below 0) -- we end our search and return eval (haven't started qs yet)
    if (depth <= 0)
        return q_search(board, alpha, beta, ply);

    // Get the TT Entry for current position
    TTEntry entry;
    uint64_t zobrists_key = board.zobrist(); 
    bool tt_hit = tt.probe(zobrists_key, entry);

    // Transposition Table cutoffs
    // Only cut with a greater or equal depth search
    if (!pv_node && entry.depth >= depth && !is_root && tt_hit && ((entry.type == NodeType::EXACT) || (entry.type == NodeType::LOWERBOUND && entry.score >= beta) || (entry.type == NodeType::UPPERBOUND && entry.score <= alpha)))
        return entry.score;

    // Internal iterative reduction. Artifically lower the depth on pv nodes / cutnodes
    // that are high enough up in the search tree that we would expect to have found
    // a Transposition. (Comment from Ethereal)
    if (pv_node && depth >= internal_iterative_deepening_depth && entry.best_move == 0)
        depth--;

    // Static evaluation for pruning metrics
    int32_t static_eval = evaluate(board);

    // Reverse futility pruning / Static Null Move Pruning
    // If eval is well above beta, we assume that it will hold
    // above beta. We "predict" that a beta cutoff will happen
    // and return eval without searching moves
    if (!pv_node && !node_is_check && depth <= reverse_futility_depth && static_eval - reverse_futility_margin * depth >= beta)
        return static_eval;

    // Null move pruning. Basically, we can assume that making a move 
    // is always better than not making our move most of the time
    // except if it's in a zugzwang. Hence, if we skip out turn and
    // we still maintain beta, then we can prune early. Also do not
    // do NMP when tt suggests that it should fail immediately
    if (!pv_node && !node_is_check && static_eval >= beta && depth >= null_move_depth && (!tt_hit || !(entry.type == NodeType::UPPERBOUND) || entry.score >= beta) && (board.hasNonPawnMaterial(Color::WHITE) || board.hasNonPawnMaterial(Color::BLACK))){
        board.makeNullMove();
        int32_t score = -alpha_beta(board, depth - null_move_reduction, -beta, -beta+1, ply + 1);
        board.unmakeNullMove();

        if (score >= beta)
            return score;
    }

    // Move ordering
    sort_moves(board, all_moves, tt_hit, entry.best_move, ply);

    // Main move loop
    // For loop is faster than foreach :)
    Move current_best_move;
    int32_t move_count = 0;

    // Clear killers of next ply
    killers[0][ply+1] = Move{}; 
    killers[1][ply+1] = Move{}; 

    for (int idx = 0; idx < all_moves.size(); idx++){

        int32_t reduction = 0;
        int32_t extension = 0;
        
        move_count++;

        Move current_move = all_moves[idx];

        bool is_noisy_move = board.isCapture(current_move);

        // Quiet late moves reduction - we have to trust that our
        // move ordering is good enough most of the time to order
        // best moves at the start
        if (!board.isCapture(current_move) && depth >= late_move_reduction_depth)
            reduction += (int32_t)(((double)late_move_reduction_base / 100) + (((double)late_move_reduction_multiplier * log(depth) * log(move_count)) / 100));

        // Basic make and undo functionality. Copy-make should be faster but that
        // debugging is for later
        board.makeMove(current_move);

        // Check extension, we increase the depth of moves that give check
        if (board.inCheck())
            extension++;

        int32_t score = 0;

        // Principle Variation Search
        if (move_count == 1)
            score = -alpha_beta(board, depth + extension - 1, -beta, -alpha, ply + 1);
        else {
            score = -alpha_beta(board, depth - reduction + extension - 1, -alpha - 1, -alpha, ply + 1);
            // Research
            if (score > alpha && score < beta) {
                score = -alpha_beta(board, depth + extension - 1, -beta, -alpha, ply + 1);
            }
        }


        board.unmakeMove(current_move);

        // Updating best_score and alpha beta pruning
        // I did not actually test this in sprt 
        if (score > best_score){
            best_score = score;
            current_best_move = current_move;

            if (is_root)
                root_best_move = current_move;

            // Update alpha
            if (score > alpha){
                alpha = score;

                // Alpha-Beta Pruning
                if (alpha >= beta){

                    // Quiet move heuristics
                    if (!is_noisy_move){
                        // Killer move heuristic
                        // We have 2 killers per ply
                        // We don't duplicate killers
                        if (current_move != killers[0][ply]){
                            killers[1][ply] = killers[0][ply]; 
                            killers[0][ply] = current_move;
                        }

                        // History Heuristic
                        bool turn = board.sideToMove() == chess::Color::WHITE;
                        int32_t from = current_move.from().index();
                        int32_t to = current_move.to().index();

                        int32_t bonus = clamp(depth * depth, -MAX_HISTORY, MAX_HISTORY);
                        quiet_history[turn][from][to] += bonus - quiet_history[turn][from][to] * abs(bonus) / MAX_HISTORY;
                    }
                    break;
                }
            }
        }
    }

    NodeType bound = best_score >= beta ? NodeType::LOWERBOUND : alpha > old_alpha ? NodeType::EXACT : NodeType::UPPERBOUND;
    uint16_t best_move_tt = bound == NodeType::UPPERBOUND ? 0 : encode_move(current_best_move.from(), current_best_move.to(), current_best_move.typeOf());

    // Storing transpositions
    tt.store(zobrists_key, best_score, depth, bound, best_move_tt);

    return best_score;

}


// Iterative deepening time management loop
// Uses soft bound time management
int32_t search_root(Board &board){
    try {
        // Aspiration window search, we predict that the score from previous searches will be
        // around the same as the next depth +/- some margin.
        int32_t score = 0;
        global_depth = 0;
        while ((global_depth == 0 || !soft_bound_time_exceeded()) && global_depth < MAX_SEARCH_DEPTH){
            // Increment the global depth since global_depth starts from 0
            global_depth++;

            int32_t alpha = DEFAULT_ALPHA;
            int32_t beta = DEFAULT_BETA;
            int32_t delta = aspiration_window_delta;
            int32_t researches = 0;
            int32_t new_score = 0;

            if (global_depth >= aspiration_window_depth){
                alpha = max(-POSITIVE_INFINITY, score - delta);
                beta = min(POSITIVE_INFINITY, score + delta);
            }
            while (true){
                new_score = alpha_beta(board, global_depth, alpha, beta, 0);
                int64_t elapsed_time = elapsed_ms();

                if (score <= alpha){
                    cout << "info depth " << global_depth << " time " << elapsed_time << " score cp " << alpha << " upperbound nodes " << total_nodes << " nps " <<   (1000 * total_nodes) / (elapsed_time + 1) << " pv " << uci::moveToUci(root_best_move) << endl;
                    beta = (alpha + beta) / 2;
                    alpha = max(-POSITIVE_INFINITY, new_score - delta);
                }
                else if (score >= beta){
                    cout << "info depth " << global_depth << " time " << elapsed_time << " score cp " << beta << " lowerbound nodes " << total_nodes << " nps " <<   (1000 * total_nodes) / (elapsed_time + 1) << " pv " << uci::moveToUci(root_best_move) << endl;
                    beta = min(POSITIVE_INFINITY, new_score + delta);
                }
                else {
                    cout << "info depth " << global_depth << " time " << elapsed_time << " score cp " << new_score << " nodes " << total_nodes << " nps " <<   (1000 * total_nodes) / (elapsed_time + 1) << " pv " << uci::moveToUci(root_best_move) << endl;
                    break;
                }

                // If we exceed our time management, we stop widening and so one last search
                if (soft_bound_time_exceeded() || researches >= maximum_aspiration_window_research){
                    delta = POSITIVE_INFINITY;
                }
                else {
                    researches++;
                    delta += delta / 2;
                }
            }

            score = new_score;
            
        }
    }

    // Hard-bound time management catch
    catch (const SearchAbort& e) { 
        
    }

    cout << "bestmove " << uci::moveToUci(root_best_move) << endl;

    return 0;
}
