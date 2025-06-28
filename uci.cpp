#include "chess.hpp"
#include <iostream>

using namespace chess;

// Process UCI Commands
// For basic SPRT functionality, we only need to implement these

///////////////// At the start ////////////////////
// gui-to-engine> uci
// engine-to-gui> id name <engine name>
// engine-to-gui> id author <your name>
// engine-to-gui> uciok
// gui-to-engine> isready
// engine-to-gui> readyok**

//////////////// During the game /////////////////
// gui-to-engine> position (startpos | fen <fen>) [moves <move1> <move2> ...]
// eg. position startpos
// eg. position startpos moves e2e4 e7e5 glf3
// eg. position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
// eg. position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 moves d5e6 a6e2 c3e2
// <wtime> - white time in ms
// <btime> - black time in ms
// <winc> - White increment in ms
// <binc> - Black increment in ms
// <movestogo> - doesn't matter (unless your chess engine uses it fot time management)
// gui-to-engine> go wtime <wtime> btime <btime> [winc <winc>] [binc <binc>] [movestogo <movestogo>]
// eg. go wtime 300000 btime 300000 winc 2000 binc 2000
// engine-to-gui> [info depth ...] // Optional
// engine-to-gui> bestmove <best move in uci notation> 

///////////////// After the game ////////////////////
// gui-to-engine> quit // quit the engine

// Full uci spec https://backscattering.de/chess/uci/

int main() {
    
}