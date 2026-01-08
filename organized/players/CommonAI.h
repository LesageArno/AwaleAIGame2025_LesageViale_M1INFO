#ifndef COMMON_AI_H
#define COMMON_AI_H

#include <string>
#include <vector>
#include "../rules/GameState.h"

#define J1_EVALUATION_FUNC "corrected" // "raw", "corrected", "defence"
#define J2_EVALUATION_FUNC "corrected" // "raw", "corrected", "defence"

// Variable to keep track of the shortest path to capture
extern int winJ1in;
extern int winJ2in;

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
