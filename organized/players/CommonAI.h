#ifndef COMMON_AI_H
#define COMMON_AI_H

#include <string>
#include <vector>
#include "../rules/GameState.h"

#define EVALUATION_FUNC "corrected" // "raw", "corrected", "defence"

// Fonctions communes aux IA
std::vector<std::string> possibleMove(GameState* game);

bool isJ1Winning(GameState* game);
bool isJ1Loosing(GameState* game);
bool isDraw(GameState* game);

int potentialCaptures(GameState* game);
float evaluate(GameState* game);
float evaluatePotentialCaptureFixed(GameState* game);
float evaluateDefence(GameState* game);


#endif
