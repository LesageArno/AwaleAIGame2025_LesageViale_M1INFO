#include "HumanPlayer.h"
#include "../rules/Rules.h"
#include <iostream>

HumanPlayer::HumanPlayer(bool isJ1) : isJ1(isJ1) {}

std::string HumanPlayer::chooseMove(GameState* game) {

    int moveFrom;
    std::string moveSeed;

    while (true) {
        if (isJ1) {
            std::cout << "[J1] You can move from holes: "
                      << "1, 3, 5, 7, 9, 11, 13, 15 "
                      << "(R, B, TR, TB): ";
        } else {
            std::cout << "[J2] You can move from holes: "
                      << "2, 4, 6, 8, 10, 12, 14, 16 "
                      << "(R, B, TR, TB): ";
        }

        std::cin >> moveFrom >> moveSeed;

        if (checkMoveValidity(moveFrom, moveSeed, game)) {
            return std::to_string(moveFrom) + " " + moveSeed;
        }

        std::cout << "Invalid move, try again.\n";
    }
}
