#include "chess.hpp"
#include "see.hpp"

using namespace chess;

// Gets the best case capture + promotion combo in a position
// doesn't matter if the capture can be made
int32_t move_best_case_value(Board& board){
    // Assume opponent has at least a pawn
    int value = see_piece_values[0];

    // Check for a higher value target
    for (int piece = 4; piece > 0; piece--){
        if (board.pieces((PieceType)(PieceType::underlying)piece, (board.sideToMove() == Color::WHITE) ? Color::BLACK : Color::WHITE)){
            // Break out of the loop if we found a piece of higher value 
            value = see_piece_values[piece];
            break;
        }
    }

    // Check for a potential pawn promotion
    if ( board.pieces(PieceType::PAWN, board.sideToMove()) & ((board.sideToMove() == Color::WHITE) ? attacks::MASK_RANK[static_cast<int>(chess::Rank::RANK_7)] : attacks::MASK_RANK[static_cast<int>(chess::Rank::RANK_2)])){
        value += see_piece_values[4] - see_piece_values[0];
    }
        
    return value;

}