#pragma once
#include <cstdint>
#include "chess.hpp"
constexpr std::int32_t see_piece_values[7] = {100, 300, 300, 500, 900, 0, 0};
bool see(chess::Board board, chess::Move move);