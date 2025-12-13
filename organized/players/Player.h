#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "../rules/GameState.h"

class Player {
public:
    virtual ~Player() = default;

    // retourne un coup sous la forme "3 r", "12 tb", ou "-" (starvation)
    virtual std::string chooseMove(GameState* game) = 0;
};

#endif
