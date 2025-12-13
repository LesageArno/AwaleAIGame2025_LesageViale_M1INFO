#include "GameState.h"

void initHole(hole* board) {
    for (int i = 0; i < BOARDSIZE; i++) {
        board[i].id = i + 1;
        board[i].redSeed = 2;
        board[i].blueSeed = 2;
        board[i].transSeed = 2;
    }
}

void initGameState(GameState* game) {
    initHole(game->board);
    game->countJ1 = 0;
    game->countJ2 = 0;
    game->countSeed = 3 * 2 * BOARDSIZE;
    game->playJ1 = true;
}
