#include "MinMaxPlayer.h"
#include "../rules/Rules.h"
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
float evaluate(GameState* game);
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

    std::cout << (isJ1 ? "[J1]" : "[J2]") << " MinMax chooses move: " << move << std::endl;

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
    if (depth == 0) return evaluate(&game);

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


std::vector<std::string> possibleMove(struct GameState* game) {
    std::vector<std::string> possible_move;
    for (int i = 0; i < BOARDSIZE; i++) {
        if ((game->playJ1 && (game->board[i].id%2 == 1)) || (!game->playJ1 && (game->board[i].id%2 == 0))) {
            if (game->board[i].redSeed != 0) {
                possible_move.push_back(std::to_string(i+1) + " r");
            }
            if (game->board[i].blueSeed != 0) {
                possible_move.push_back(std::to_string(i+1) + " b");
            }
            if (game->board[i].transSeed != 0) {
                possible_move.push_back(std::to_string(i+1) + " tr");
                possible_move.push_back(std::to_string(i+1) + " tb");
            }
        }
    }
    return possible_move;
}

bool isJ1Winning(struct GameState* game) {
    // If J1 has the absolute majority of the seed, win 
    if (game->countJ1 >= 49) {
        return true;
    }

    // If J1 has the relative majority of the seed when less than 10 seed, win
    if (game->countSeed < 10) {
        if (game->countJ1 > game->countJ2) {
            return true;
        }
    }

    // Else do not win
    return false;
}
bool isJ1Loosing(struct GameState* game) {
    // If J2 has the absolute majority of the seed, loose 
    if (game->countJ2 >= 49) {
        return true;
    }

    // If J2 has the relative majority of the seed when less than 10 seed, loose
    if (game->countSeed < 10) {
        if (game->countJ2 > game->countJ1) {
            return true;
        }
    }

    // Else do not loose
    return false;
}
bool isDraw(struct GameState* game) {
    // If J1 equal J2 the majority of the seed when less than 10 seed, loose
    if ((game->countSeed < SEED_LIMIT) && (game->countJ1 == game->countJ2)) {
        return true;
    }

    // Else, not draw
    return false;
}

float evaluate(struct GameState* game){
    struct hole* board = game->board;
    float score = 0.0f;
    bool isP1 = game->playJ1;

    // Total seeds per player (in their 8 holes)
    int p1Seedcount = 0;
    int p1Seedtrans = 0;
    for (int i = 0; i < BOARDSIZE ; i += 2){
        p1Seedcount += board[i].redSeed + board[i].blueSeed + board[i].transSeed;
        p1Seedtrans += board[i].transSeed;
    }

    int p2Seedcount = 0;
    int p2Seedtrans = 0;
    for (int i = 1; i < BOARDSIZE ; i += 2){
        p2Seedcount += board[i].redSeed + board[i].blueSeed + board[i].transSeed;
        p2Seedtrans += board[i].transSeed;
    }

    int seedsDiff = isP1
        ? (p1Seedcount - p2Seedcount)
        : (p2Seedcount - p1Seedcount);
    int transDiff = isP1
        ? (p1Seedtrans - p2Seedtrans)
        : (p2Seedtrans - p1Seedtrans);

    int countDiff = (game->countJ1 - game->countJ2);
    if(!isP1) countDiff = -countDiff;
    int potential = potentialCaptures(game);

    // NORMALIZATION
    float normSeedsDiff = seedsDiff / 48.0f; //3c*8h*2p/2 -> 48 seeds
    float normTransDiff = transDiff / 16.0f; //16 possible transmuted seeds in one half of the board (1/hole)
    float normCountDiff = countDiff / 48.0f; //48 seeds maximum at game start (24*2)
    float normPotential = potential / 16.0f; //estimated max 16 seeds capturable in a chain

    // AUTRES PARAMÈTRES (à ajouter éventuellement) :
    // - potentiel de starvation : vérifier si on laisse l’adversaire sans coup
    // - coups dangereux : est-ce que ce coup ouvre des captures à l’adversaire ?
    // - potentiel de blocage : empêche l’adversaire de capturer à son tour
    // - évaluer la vulnérabilité de nos trous à 1–2 graines

    // weighted evaluation
    score =
        0.4f * normCountDiff +
        0.3f * normPotential +
        0.2f * normSeedsDiff +
        0.1f * normTransDiff;

    // clamp -1 ; 1
    if(score > 1.0f) score = 1.0f;
    if(score < -1.0f) score = -1.0f;

    return score;
}
int potentialCaptures(struct GameState* game){
    int potential = 0;

    //loop on every holes of the player
    for (int i = (game->playJ1 ? 0 : 1); i < BOARDSIZE; i += 2) {
    
        int colors[3] = {game->board[i].redSeed, game->board[i].blueSeed, game->board[i].transSeed};
        for (int c = 0; c < 3; ++c) {
            int seedsToPlay = colors[c];
            if (seedsToPlay == 0) continue;

            // simulate without modifying the real board
            int tempBoard[BOARDSIZE];
            for (int k = 0; k < BOARDSIZE; ++k)
                tempBoard[k] = game->board[k].redSeed + game->board[k].blueSeed + game->board[k].transSeed;

            int currentPos = i;
            int transSeeds = (c == 2 ? game->board[i].transSeed : 0);
            int colorSeeds = seedsToPlay - transSeeds;

            tempBoard[i] -= seedsToPlay;

            // while thereare seeds
            while (transSeeds > 0 || colorSeeds > 0) {
                currentPos = (currentPos + 1) % BOARDSIZE;

                // first transeeds
                if (transSeeds > 0) {
                    tempBoard[currentPos] += 1;
                    transSeeds--;
                } 
                // after colorseeds
                else if (colorSeeds > 0) {
                    tempBoard[currentPos] += 1;
                    colorSeeds--;
                }
            }


            // from last pos
            int pos = currentPos;
            while (true) {
                int totalSeeds = tempBoard[pos]; 

                // capture
                if (totalSeeds == 2 || totalSeeds == 3) {
                    potential += totalSeeds;
                    tempBoard[pos] = 0;

                    // reculer d'une case pour vérifier la capture en chaîne
                    pos = (pos - 1 + BOARDSIZE) % BOARDSIZE;
                }
                else {
                    break;
                }
            }

        }

    }

    return potential;
}




