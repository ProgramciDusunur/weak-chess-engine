#pragma once

#include <algorithm> 

#include "chess.hpp"
#include "ordering.hpp"
#include "transposition.hpp"

// Move ordering
inline void sort_moves(chess::Movelist& movelist, bool tt_hit, uint16_t tt_move) {
    std::sort(movelist.begin(), movelist.end(), [tt_move, tt_hit](const chess::Move& a, const chess::Move& b) {
        bool a_is_tt = encode_move(a.from(), a.to(), a.typeOf()) == tt_move;
        bool b_is_tt = encode_move(b.from(), b.to(), b.typeOf()) == tt_move;

        if (tt_hit && a_is_tt != b_is_tt)
            return a_is_tt > b_is_tt;

        return false;
    });
}
