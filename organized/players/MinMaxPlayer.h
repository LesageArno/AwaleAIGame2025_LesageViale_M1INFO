#ifndef MINMAXPLAYER_H
#define MINMAXPLAYER_H

#include "Player.h"

class MinMaxPlayer : public Player {
public:
    explicit MinMaxPlayer(bool isJ1, int depth = 4);

    std::string chooseMove(GameState* game) override;

private:
    bool isJ1;
    int pmax; // profondeur MinMax
    bool verbose;
};

#endif
