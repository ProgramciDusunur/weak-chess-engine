#pragma once
#include <stdint.h>

#include "packing.hpp"

// Tapered static evaluation function given a board position
// returns score relative to player
int32_t evaluate(const chess::Board& board);