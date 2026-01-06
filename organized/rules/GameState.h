#ifndef GAMESTATE_H
#define GAMESTATE_H

#define BOARDSIZE 16
#define SEED_LIMIT 10

struct hole {
    int id;
    int redSeed;
    int blueSeed;
    int transSeed;
};

struct GameState {
    hole board[BOARDSIZE];
    int countJ1;
    int countJ2;
    int countSeed;
    bool playJ1;
};

void initHole(hole* board);
void initGameState(GameState* game);

#endif
