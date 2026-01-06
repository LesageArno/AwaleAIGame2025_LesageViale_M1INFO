#ifndef COMMON_AI_H
#define COMMON_AI_H

#include <string>
#include <vector>
#include "../rules/GameState.h"

// Fonctions communes aux IA
std::vector<std::string> possibleMove(GameState* game);

bool isJ1Winning(GameState* game);
bool isJ1Loosing(GameState* game);
bool isDraw(GameState* game);

int potentialCaptures(GameState* game);
float evaluate(GameState* game);
float evaluate2(GameState* game);

#endif
