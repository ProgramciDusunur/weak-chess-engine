#pragma once
#include "chess.hpp"

// Prints the bitboard, nothing else
void print_bitboard(chess::Bitboard x){
    int32_t display_board[64]{};
    while (x){
        int square = x.pop();
        display_board[square^56] = 1;
    }

    for (int i = 0; i < 64; i++){

        // To format the board to look like a chess board, we need to endline
        // every time a chess row ends, ie after the "h" file
        if (i != 0 && i % 8 == 0)
            std::cout << "\n";

        std::cout << display_board[i] << " ";
    }
    std::cout << "\n\n";
}
