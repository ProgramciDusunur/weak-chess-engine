#include <array>
#include <cstdint>
#include <utility>
#include <cassert>
#include <vector>

#include "chess.hpp"
#include "ordering.hpp"
#include "transposition.hpp"
#include "mvv_lva.hpp"
#include "see.hpp"
#include "search_info.hpp"
#include "history.hpp"

using namespace chess;

constexpr int32_t TT_BONUS = 1000000;
constexpr int32_t KILLER_BONUS = 90000;

void sort_moves(Board& board, Movelist& movelist, bool tt_hit, uint16_t tt_move, int32_t ply, SearchInfo search_info) {

    int32_t parent_move_piece = search_info.parent_move_piece;
    int32_t parent_move_square = search_info.parent_move_square;
    int32_t parent_parent_move_piece = search_info.parent_parent_move_piece;
    int32_t parent_parent_move_square = search_info.parent_parent_move_square;

    const size_t move_count = movelist.size();
    assert(move_count <= 256); 

    // Score array for corresponding moves
    std::array<int32_t, 256> scores;

    for (size_t i = 0; i < move_count; i++) {
        const auto& move = movelist[i];
        int32_t score = 0;

        if (tt_hit && move.move() == tt_move) {
            // TT-Moves ordering
            score = TT_BONUS;
        } else if (board.isCapture(move)) {
            // MVV-LVA ordering
            score = mvv_lva(board, move);
            
            // See ordering, put all bad captures at the far end of the ordered list
            // by making its value a really big negative number
            score += see(board, move, 0) ? 0 : -10000000;

        } else if (killers[0][ply] == move || killers[1][ply] == move) {
            // Killer move history
            score = KILLER_BONUS;
        } else {
            score = quiet_history[board.sideToMove() == Color::WHITE][move.from().index()][move.to().index()];

            // Countermoves
            if (parent_move_piece != -1 && parent_move_square != -1)
                score += one_ply_conthist[parent_move_piece][parent_move_square][static_cast<int32_t>(board.at(move.from()).internal())][move.to().index()];

            // Follow-up moves
            if (parent_parent_move_piece != -1 && parent_parent_move_square != -1)
                score += two_ply_conthist[parent_parent_move_piece][parent_parent_move_square][static_cast<int32_t>(board.at(move.from()).internal())][move.to().index()];

        }

        scores[i] = score;
    }

    // In-place sort (descending order) using scores
    for (size_t i = 0; i < move_count; i++) {
        size_t max_idx = i;
        for (size_t j = i + 1; j < move_count; j++) {
            if (scores[j] > scores[max_idx]) {
                max_idx = j;
            }
        }
        if (max_idx != i) {
            std::swap(scores[i], scores[max_idx]);
            std::swap(movelist[i], movelist[max_idx]);
        }
    }
}

// Special captures sorting
// returns a boolean vector matching the see bool result of each capture
std::vector<bool> sort_captures(Board& board, Movelist& movelist, bool tt_hit, std::uint16_t tt_move) {
    const size_t move_count = movelist.size();
    assert(move_count <= 256);

    std::array<int32_t, 256> scores;
    std::array<bool, 256> sees;

    for (size_t i = 0; i < move_count; i++) {
        const auto& move = movelist[i];
        int32_t score = 0;
        bool good_see = see(board, move, 0);

        if (tt_hit && move.move() == tt_move) {
            score = TT_BONUS;
        } else {
            score = mvv_lva(board, move);
            score += good_see ? 0 : -10000000;
        }

        scores[i] = score;
        sees[i] = good_see;
    }

    // In-place selection sort on scores (and sync sees + movelist)
    for (size_t i = 0; i < move_count; i++) {
        size_t max_idx = i;
        for (size_t j = i + 1; j < move_count; j++) {
            if (scores[j] > scores[max_idx]) {
                max_idx = j;
            }
        }
        if (max_idx != i) {
            std::swap(scores[i], scores[max_idx]);
            std::swap(sees[i], sees[max_idx]);
            std::swap(movelist[i], movelist[max_idx]);
        }
    }

    return std::vector<bool>(sees.begin(), sees.begin() + move_count);
}
