#ifndef RULES_H
#define RULES_H

#include <string>
#include "GameState.h"

// display
void printBoard(const GameState* game);
void printState(const GameState* game);

// rules
bool checkMoveValidity(int moveFrom, std::string moveSeed, GameState* game);
bool checkAvailableMove(GameState* game);

int playRed(int moveFrom, bool trans, GameState* game);
int playBlue(int moveFrom, bool trans, GameState* game);

void capture(int lastMove, GameState* game, bool verbose);
int starvingCapture(GameState* game);

// moteur de jeu
//void playTurn(GameState* game, int moveFrom, const std::string& moveSeed);

GameState Apply(GameState game, std::string move);

#endif
