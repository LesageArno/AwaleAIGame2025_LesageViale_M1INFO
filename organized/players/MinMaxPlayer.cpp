#include "MinMaxPlayer.h"
#include "../rules/Rules.h"
#include "CommonAI.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// fonctions internes
std::string DecisionMinMax(GameState* game, int pmax);
float MinMaxRec(GameState game, bool isMax, int depth);
std::vector<std::string> possibleMove(GameState* game);
bool isJ1Winning(GameState* game);
bool isJ1Loosing(GameState* game);
bool isDraw(GameState* game);
int potentialCaptures(GameState* game);

// constructeur et chooseMove

MinMaxPlayer::MinMaxPlayer(bool isJ1, int depth) : isJ1(isJ1), pmax(depth) {}

std::string MinMaxPlayer::chooseMove(GameState* game) {
    // Vérifier si c'est bien le tour du joueur
    if (game->playJ1 != isJ1) {
        std::cerr << "Erreur : MinMaxPlayer joue hors de son tour !" << std::endl;
        return "-";
    }

    // Appeler ta fonction DecisionMinMax
    std::string move = DecisionMinMax(game, pmax);

    if(verbose) { std::cout << (isJ1 ? "[J1]" : "[J2]") << " MinMax chooses move: " << move << std::endl; }

    return move;
}

// fonctions MinMax

std::string DecisionMinMax(GameState* game, int pmax) {
    
    // Note that best move and worst move are relative to J1 so worst move of J1 is best move of J2,...
    std::map<std::string, float> value;
    float bestVal = -2.;
    float val;
    std::string bestMove;

    // If no move, then  return that the only move possible is starving
    if (!checkAvailableMove(game)) {
        return "-";
    }

    // Move selection
    for (std::string move : possibleMove(game)) {
        val = MinMaxRec(Apply(*game, move), false, pmax);
        if (val > bestVal) {
            bestVal = val;
            bestMove = move;
        }
    }

    return bestMove;
}

float MinMaxRec(GameState game, bool isMax, int depth) {
    // Cas terminaux
    if (isJ1Winning(&game)) return 1.0f;
    if (isJ1Loosing(&game)) return -1.0f;
    if (isDraw(&game)) return 0.0f;
    if (depth == 0) {
        if (EVALUATION_FUNC == "raw") {
            return evaluate(&game); // Always from J1's perspective
        } else if (EVALUATION_FUNC == "corrected") {
            return evaluatePotentialCaptureFixed(&game);
        } else if (EVALUATION_FUNC == "defence") {
            return evaluateDefence(&game);
        }
    }

    // Si pas de coups disponibles -> famine/starvation
    if (!checkAvailableMove(&game)) {
        return MinMaxRec(Apply(game, "-"), !isMax, depth - 1);
    }

    if (isMax) {
        float maxVal = -2.0f;
        for (const auto &move : possibleMove(&game)) {
            float val = MinMaxRec(Apply(game, move), !isMax, depth - 1);
            if (val > maxVal) maxVal = val;
        }
        return maxVal;
    } else {
        float minVal = 2.0f;
        for (const auto &move : possibleMove(&game)) {
            float val = MinMaxRec(Apply(game, move), !isMax, depth - 1);
            if (val < minVal) minVal = val;
        }
        return minVal;
    }
}
