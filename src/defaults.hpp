#pragma once
#include <cstdint>
#include <string>
#include <algorithm>
#include <iostream>

struct SearchParam {
    std::string name;
    int32_t current;
    int32_t min;
    int32_t max;
    int32_t default_value;

    // Constructor: name first
    SearchParam(const std::string& name, int32_t current, int32_t min, int32_t max, int32_t def)
        : name(name), current(current), min(min), max(max), default_value(def) {}

    // Set current value (clamped to min/max)
    void set(int32_t value) {
        current = std::clamp(value, min, max);
    }

    // Reset current to default
    void reset() {
        current = default_value;
    }

    // Debug string
    std::string to_string() const {
        return name + ": current=" + std::to_string(current) +
               ", min=" + std::to_string(min) +
               ", max=" + std::to_string(max) +
               ", default=" + std::to_string(default_value);
    }

    // UCI option printout
    void print_uci_option() const {
        std::cout << "option name " << name
                  << " type spin"
                  << " default " << default_value
                  << " min " << min
                  << " max " << max
                  << std::endl;
    }
};

// Example usage: name, current, min, max, default
SearchParam tt_size("Hash", 64, 1, 16384, 64);
SearchParam threads("Threads", 1, 1, 1, 1);
