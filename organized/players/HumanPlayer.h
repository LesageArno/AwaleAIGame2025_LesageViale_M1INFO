#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "Player.h"

class HumanPlayer : public Player {
public:
    explicit HumanPlayer(bool isJ1);

    std::string chooseMove(GameState* game) override;

private:
    bool isJ1;
};

#endif
