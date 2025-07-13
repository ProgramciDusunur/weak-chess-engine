#include <cstdint>
#include "chess.hpp"
#include "search.hpp"
#include "history.hpp"

using namespace chess;

// Histories
Move killers[2][MAX_SEARCH_PLY+1]{};
int32_t quiet_history[2][64][64]{};
int32_t one_ply_conthist[12][64][12][64]{};
int32_t two_ply_conthist[12][64][12][64]{};

// Reset killer moves
void reset_killers(){
    for (int32_t i = 0; i < 2; ++i)
        for (int32_t j = 0; j < MAX_SEARCH_PLY + 1; ++j)
            killers[i][j] = chess::Move{};
}


// Reset quiet histiry
void reset_quiet_history() {
    for (int32_t color = 0; color < 2; ++color) {
        for (int32_t piece = 0; piece < 64; ++piece) {
            for (int32_t square = 0; square < 64; ++square) {
                quiet_history[color][piece][square] = 0;
            }
        }
    }
}

// Reset continuation history
void reset_continuation_history() {
    for (int32_t prev = 0; prev < 12; ++prev) {
        for (int32_t prev_sq = 0; prev_sq < 64; ++prev_sq) {
            for (int32_t curr = 0; curr < 12; ++curr) {
                for (int32_t curr_sq = 0; curr_sq < 64; ++curr_sq){
                    one_ply_conthist[prev][prev_sq][curr][curr_sq] = 0;
                    two_ply_conthist[prev][prev_sq][curr][curr_sq] = 0;
                }
            }
        }
    }
}