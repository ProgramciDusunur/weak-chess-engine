#pragma once
#include <stdint.h>

// Packs two 16-bit scores (midgame, endgame) into a single 32-bit integer.
inline constexpr int32_t S(int16_t mg, int16_t eg) {
    return ((int32_t) eg << 16) + (int32_t) mg;
}

// Extracts the midgame component from packed eval.
inline int16_t unpack_mg(int32_t packed) {
    return (int16_t) packed;
}

// Extracts the endgame component from packed eval.
inline int16_t unpack_eg(int32_t packed) {
    return (int16_t) ((packed + 0x8000) >> 16);
}
