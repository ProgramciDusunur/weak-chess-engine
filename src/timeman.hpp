#pragma once
#include <cstdint>
#include <chrono>

// Time management constants 
constexpr int64_t SOFT_TM_RATIO = 40;
constexpr int64_t HARD_TM_RATIO = 10;

// Time tracking
extern int64_t max_soft_time_ms;
extern int64_t max_hard_time_ms;
extern std::chrono::time_point<std::chrono::system_clock> search_start_time;

// Get's the epased time after searching
inline int64_t elapsed_ms() {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - search_start_time);
    return elapsed.count();
}

// Returns true if time exceeds hard bound time limit
inline bool hard_bound_time_exceeded() {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - search_start_time);
    return elapsed.count() > max_hard_time_ms;
}