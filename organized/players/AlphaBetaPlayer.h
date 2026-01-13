#ifndef ALPHABETAPLAYER_H
#define ALPHABETAPLAYER_H

#include "../rules/GameState.h"
#include "Player.h"
#include <string>

class AlphaBetaPlayer : public Player {
private:
    bool isJ1;
    int pmax;
    bool verbose;
public:
    AlphaBetaPlayer(bool isJ1, int depth, bool verbose = false); 
    std::string chooseMove(GameState* game) override;
};

#endif
