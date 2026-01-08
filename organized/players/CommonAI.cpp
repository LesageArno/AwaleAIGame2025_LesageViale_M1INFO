#include "CommonAI.h"
#include "../rules/Rules.h"
#include <algorithm>

std::vector<std::string> possibleMove(struct GameState* game) {
    std::vector<std::string> possible_move;
    for (int i = 0; i < BOARDSIZE; i++) {
        if ((game->playJ1 && (game->board[i].id%2 == 1)) || (!game->playJ1 && (game->board[i].id%2 == 0))) {
            if (game->board[i].redSeed != 0) {
                possible_move.push_back(std::to_string(i+1) + " R");
            }
            if (game->board[i].blueSeed != 0) {
                possible_move.push_back(std::to_string(i+1) + " B");
            }
            if (game->board[i].transSeed != 0) {
                possible_move.push_back(std::to_string(i+1) + " TR");
                possible_move.push_back(std::to_string(i+1) + " TB");
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

float evaluatePotentialCaptureFixed(struct GameState* game){
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

    int seedsDiff = p1Seedcount - p2Seedcount;
    int transDiff = p1Seedtrans - p2Seedtrans;

    int countDiff = game->countJ1 - game->countJ2;
    int potential = potentialCaptures(game);
    potential = isP1 ? potential : -potential; // Potential is always positive, so we negate it to avoid problems

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

float evaluateDefence(struct GameState* game) {
    struct hole* board = game->board;
    float score = 0.0f;
    bool isP1 = game->playJ1; 
    int countSeed;

    // Total seeds per player (in their 8 holes)
    int p1Seedcount = 0;
    int p1Seedtrans = 0;
    int p1OneOrTwoSeed = 0;
    for (int i = 0; i < BOARDSIZE ; i += 2){
        countSeed = board[i].redSeed + board[i].blueSeed + board[i].transSeed;
        p1Seedcount += countSeed;
        p1Seedtrans += board[i].transSeed;
        
        // We want to minimize the number of holes that can be captured.
        if (countSeed == 1 || countSeed == 2) {
            p1OneOrTwoSeed++;
        }
    }

    int p2Seedcount = 0;
    int p2Seedtrans = 0;
    int p2OneOrTwoSeed = 0;
    for (int i = 1; i < BOARDSIZE ; i += 2){
        countSeed = board[i].redSeed + board[i].blueSeed + board[i].transSeed;
        p2Seedcount += countSeed;
        p2Seedtrans += board[i].transSeed;
    
        // We want to minimize the number of holes that can be captured for J2.
        if (countSeed == 1 || countSeed == 2) {
            p2OneOrTwoSeed++;
        }
    }

    int seedsDiff = p1Seedcount - p2Seedcount;
    int transDiff = p1Seedtrans - p2Seedtrans;

    int countDiff = (game->countJ1 - game->countJ2);
    int potential = potentialCaptures(game);
    potential = isP1 ? potential : -potential; // Potential is always positive, so we negate it to avoid problems

    // NORMALIZATION
    float normDefence = isP1 ? 
        1 - p1OneOrTwoSeed/16.0f : // Proportion of holes that cannot be capture for J1
        - (1 - p2OneOrTwoSeed/16.0f) ; // Proportion of holes that cannot be captured for J2
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
        0.25f * normDefence + 
        0.3f * normPotential +
        0.25f * normCountDiff +
        0.06f * normSeedsDiff +
        0.04f * normTransDiff;

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
