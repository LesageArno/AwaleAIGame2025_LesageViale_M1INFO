// AlphaBetaPlayer.cpp
#include "AlphaBetaPlayer.h"
#include "CommonAI.h"
#include "../rules/Rules.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

float AlphaBeta(GameState game, float alpha, float beta, bool isMax, int pmax);
std::string DecisionAlphaBeta(GameState* game, int pmax);
int findBestPmax(GameState* game);

AlphaBetaPlayer::AlphaBetaPlayer(bool isJ1, int depth, bool verbose)
    : isJ1(isJ1), pmax(depth), verbose(verbose) {}

std::string AlphaBetaPlayer::chooseMove(GameState* game) {
    if (game->playJ1 != isJ1) {
        if (verbose) std::cerr << "Erreur : AlphaBetaPlayer joue hors de son tour !" << std::endl;
        return "-";
    }

    std::string move = DecisionAlphaBeta(game, pmax==-1 ? findBestPmax(game) : pmax);

    if (verbose)
        std::cout << (isJ1 ? "[J1]" : "[J2]") 
                  << " AlphaBeta chooses move: " << move << std::endl;

    return move;
}

//////////////////////////////////////////////////////////////////////////

float AlphaBeta(GameState game, float alpha, float beta, int depth) {
    // J1 is always maximizing
    bool isMax = game.playJ1; 

    if (isJ1Winning(&game)) return 1.0f;
    if (isJ1Loosing(&game)) return -1.0f;
    if (isDraw(&game)) return 0.0f;
    if (depth == 0) {
        if (((J1_EVALUATION_FUNC == "raw") && isMax) || ((J2_EVALUATION_FUNC == "raw") && !isMax)) {
            return evaluate(&game); // Always from J1's perspective
        } else if (((J1_EVALUATION_FUNC == "corrected") && isMax) || ((J2_EVALUATION_FUNC == "corrected") && !isMax)) {
            return evaluatePotentialCaptureFixed(&game);
        } else if (((J1_EVALUATION_FUNC == "defence") && isMax) || ((J2_EVALUATION_FUNC == "defence") && !isMax)) {
            return evaluateDefence(&game);
        }
    }    

    // Handle starvation
    if (!checkAvailableMove(&game)) {
        GameState nextGame = Apply(game, "-");
        return AlphaBeta(nextGame, alpha, beta, depth - 1);
    }

    // MAXIMIZING for J1
    if (isMax) {
        float value = -2.0f;
        for (auto &move : possibleMove(&game)) {
            GameState next = Apply(game, move);
            value = std::max(value, AlphaBeta(next, alpha, beta, depth - 1));
            alpha = std::max(alpha, value);
            if (alpha >= beta) break; // beta cut
        }
        return value;
    }

    // MINIMIZING for J2
    else {
        float value = 2.0f;
        for (auto &move : possibleMove(&game)) {
            GameState next = Apply(game, move);
            value = std::min(value, AlphaBeta(next, alpha, beta, depth - 1));
            beta = std::min(beta, value);
            if (beta <= alpha) break; // alpha cut
        }
        return value;
    }
}

std::string DecisionAlphaBeta(GameState* game, int depth) {
    float alpha = -2.0f;
    float beta  =  2.0f;

    bool isMax = game->playJ1;

    std::string bestMove;
    float bestVal = isMax ? -2.0f : 2.0f;

    if (!checkAvailableMove(game)) return "-";

    for (auto &move : possibleMove(game)) {
        GameState next = Apply(*game, move);
        float val = AlphaBeta(next, alpha, beta, depth - 1);

        if (isMax) {  // J1 turn
            if (val > bestVal) {
                bestVal = val;
                bestMove = move;
                //std::cout << bestMove << " : " << bestVal << std::endl;
            }
            alpha = std::max(alpha, bestVal);
        }
        else {        // J2 turn
            if (val < bestVal) {
                bestVal = val;
                bestMove = move;
                //std::cout << bestMove << " : " << bestVal << std::endl;
            }
            beta = std::min(beta, bestVal);
        }
    }

    return bestMove;
}

int findBestPmax(GameState* game) {
    static float seedMaxQte = 3.*2*BOARDSIZE;
    float removePercent = 1 - (float)game->countSeed/seedMaxQte;
    if (removePercent < 0.15) {
        return 6;
    } else if (removePercent < 0.30) {
        return 7;
    } else if (removePercent < 0.7) {
        return 8;
    } else if (removePercent < 0.8) {
        return 9;
    } else {
        return 10;
    }
}
