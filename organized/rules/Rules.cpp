#include "Rules.h"
#include "GameState.h"
#include <iostream>
#include <cstdio>

// ---- display ----

// Board representation method
void printBoard(const GameState* game) {
    const hole* board = game->board;
    
    std::cout << "|--------|--------|--------|--------|--------|--------|--------|--------|" << std::endl;

    for (int i = 0; i<BOARDSIZE/2; i++) {
        printf("|%02d\033[41m%02d\033[44m%02d\033[43m%02d\033[0m", i+1, board[i].redSeed, board[i].blueSeed, board[i].transSeed);
    }
    std::cout << "|" << std::endl << "|--------|--------|--------|--------|--------|--------|--------|--------|" << std::endl;
    for (int i = BOARDSIZE-1; i>=BOARDSIZE/2; i--) {
        printf("|%02d\033[41m%02d\033[44m%02d\033[43m%02d\033[0m", i+1, board[i].redSeed, board[i].blueSeed, board[i].transSeed);
    }
    std::cout << "|" << std::endl << "|--------|--------|--------|--------|--------|--------|--------|--------|" << std::endl;
    std::cout << "Score: [J1] " << game->countJ1 << " vs [J2] " << game->countJ2 << std::endl;
}

// State representation method
void printState(const GameState* game) {
    const hole* board = game->board;
    for (int i = 0; i<BOARDSIZE; i++) {
        std::cout << "id: " << board[i].id << ", redSeed: " << board[i].redSeed << ", blueSeed: " << board[i].blueSeed << ", transSeed: " << board[i].transSeed << std::endl;
    }
}

// ---- rules ----

// Play red function
int playRed(int moveFrom, bool trans, struct GameState* game) {
    // Add red seed to the sum
    int redPlace = game->board[moveFrom-1].redSeed;
    game->board[moveFrom-1].redSeed = 0;

    // Add transparent seed to the sum if requested
    int transPlace = 0;
    if (trans) {
        transPlace = game->board[moveFrom-1].transSeed;
        game->board[moveFrom-1].transSeed = 0;
    }

    // Initialise current position to next cell mod 16
    int currentPosition = moveFrom%BOARDSIZE;

    // Loop until no more seed are to place
    while ((transPlace > 0) || (redPlace > 0)) {
        if (moveFrom-1==currentPosition) {
            currentPosition = (currentPosition + 1)%BOARDSIZE;
        }

        // Place transparent first then red
        if (transPlace > 0) {
            transPlace--;
            game->board[currentPosition].transSeed = game->board[currentPosition].transSeed + 1;
        } else if (redPlace > 0) {
            redPlace--;
            game->board[currentPosition].redSeed = game->board[currentPosition].redSeed + 1;
        }

        // Go to next hole
        // Go to next hole if seeds are remaining
        if ((transPlace > 0) || (redPlace > 0)) {
            currentPosition = (currentPosition + 1)%BOARDSIZE;
        }
    }
    // Return last position + 1 to return in range 1-16
    return currentPosition + 1;
}

// Play blue function
int playBlue(int moveFrom, bool trans, struct GameState* game) {
    // Add blue seed to the sum
    int bluePlace = game->board[moveFrom-1].blueSeed;
    game->board[moveFrom-1].blueSeed= 0;

    // Add transparent seed to the sum if requested
    int transPlace = 0;
    if (trans) {
        transPlace = game->board[moveFrom-1].transSeed;
        game->board[moveFrom-1].transSeed = 0;
    }

    // Initialise current position to next cell mod 16
    int currentPosition = moveFrom%BOARDSIZE;

    // Loop until no more seed are to place
    while ((transPlace > 0) || (bluePlace > 0)) {
        // Place transparent first then blue
        if (transPlace > 0) {
            transPlace--;
            game->board[currentPosition].transSeed = game->board[currentPosition].transSeed + 1;
        } else if (bluePlace > 0) {
            bluePlace--;
            game->board[currentPosition].blueSeed = game->board[currentPosition].blueSeed + 1;
        }
        // Go to next opponent position if seeds are remaining
        if ((transPlace > 0) || (bluePlace > 0)) {
            currentPosition = (currentPosition + 2)%BOARDSIZE;
        }
    }

    // Return last position + 1 to return in range 1-16
    return currentPosition + 1;
}

// Check move validity function
bool checkMoveValidity(int moveFrom, std::string moveSeed, struct GameState* game) {
    // Check for illegal move (move outside the board)
    if ((moveFrom > BOARDSIZE) || (moveFrom < 1)) {
        return false;
    }

    // Check for illegal move (move from opponent position) 
    if ((game->playJ1 && (moveFrom%2 == 0))||(!game->playJ1 && (moveFrom%2 == 1))) {
        return false;
    }

    // Check if redSeed are remaining in the hole
    if (moveSeed=="r") {
        if (game->board[moveFrom-1].redSeed==0) {
            return false;
        }
        return true;
    }

    // Check if blueSeed are remaining in the hole
    if (moveSeed=="b") {
        if (game->board[moveFrom-1].blueSeed==0) {
            return false;
        }
        return true;
    }

    // Check if transSeed are remaining in the hole
    if ((moveSeed=="tr") || (moveSeed=="tb")) {
        if (game->board[moveFrom-1].transSeed==0) {
            return false;
        }
        return true;
    }

    // Illegal seed
    return false;
}

// Check if any available move.
bool checkAvailableMove(struct GameState* game) {
    for (int i = 0; i < BOARDSIZE; i++) {

        // If it's the turn of J1 and the hole of J1, check whether it contains anything 
        if (game->playJ1 && ((i+1) % 2 == 1)) {
            if (game->board[i].blueSeed + game->board[i].transSeed + game->board[i].redSeed > 0) {
                return true;
            }

        // Same for J2
        } else if (!game->playJ1 && ((i+1) % 2 == 0)) {
            if (game->board[i].blueSeed + game->board[i].transSeed + game->board[i].redSeed > 0) {
                return true;
            }
        }
    }

    // If no hole are available for a particular player, then starvation.
    return false;
}

// Capture Method
void capture(int lastMove, struct GameState* game, bool verbose) {
    // Loop until we cannot capture (remember lastMove is initially position in array + 1)
    int counter = 1;
    while (true) {

        // If we go bellow zero for capturing, then return to the opposite side of the board
        if (lastMove-counter == -1) {
            lastMove = BOARDSIZE;
            counter = 1;
        }

        // Compute gain for the current hole
        int holeTotal = game->board[lastMove - counter].redSeed + game->board[lastMove - counter].blueSeed + game->board[lastMove - counter].transSeed;
        
        // If the hole goes to 2 or 3 seeds, capture otherwise stop
        if ((holeTotal == 2) || (holeTotal == 3)) {
            // Capture
            game->board[lastMove - counter].redSeed = 0;
            game->board[lastMove - counter].blueSeed = 0;
            game->board[lastMove - counter].transSeed = 0;

            // Update counter for seeds
            if (game->playJ1) {
                if (verbose) {
                    std::cout << "[J1] Capture hole: " << game->board[lastMove - counter].id << " for " << holeTotal << " seeds." << std::endl;
                }
                game->countJ1 = game->countJ1 + holeTotal;
            } else {
                if (verbose) {
                    std::cout << "[J2] Capture hole " << game->board[lastMove - counter].id << " for " << holeTotal << " seeds." << std::endl;
                }
                game->countJ2 = game->countJ2 + holeTotal;
            }
            game->countSeed = game->countSeed - holeTotal;
        } else {
            break;
        }
        // Update position counter
        counter++;
    }
}

// Retrieve seed for opposite player when starving
int starvingCapture(struct GameState* game) {
    int seedCount = 0;
    for (int i = 0; i < BOARDSIZE; i++) {
        if (game->playJ1 && ((i+1) % 2 == 1)) {
            seedCount = seedCount + game->board[i].blueSeed + game->board[i].redSeed + game->board[i].transSeed;
        } else if (!game->playJ1 && ((i+1) % 2 == 0)) {
            seedCount = seedCount + game->board[i].blueSeed + game->board[i].redSeed + game->board[i].transSeed;
        }
    }
    if (game->playJ1) {game->countJ1 = game->countJ1 + seedCount;}
    if (!game->playJ1) {game->countJ2 = game->countJ2 + seedCount;}
    return seedCount;
}


GameState Apply(GameState game, std::string move) {
    GameState gameCopy = game;

    // === Cas starvation ===
    if (move == "-") {
        starvingCapture(&gameCopy);
        gameCopy.playJ1 = !gameCopy.playJ1;
        return gameCopy;
    }

    // === Décodage du coup ===
    int sep = move.find(' ');
    int moveFrom = std::stoi(move.substr(0, sep));
    std::string color = move.substr(sep + 1);

    int lastPos;

    // === Semis ===
    if (color == "r") {
        lastPos = playRed(moveFrom, false, &gameCopy);
    }
    else if (color == "tr") {
        lastPos = playRed(moveFrom, true, &gameCopy);
    }
    else if (color == "b") {
        lastPos = playBlue(moveFrom, false, &gameCopy);
    }
    else if (color == "tb") {
        lastPos = playBlue(moveFrom, true, &gameCopy);
    }

    // === Capture ===
    capture(lastPos, &gameCopy, false);

    // === Changement de joueur ===
    gameCopy.playJ1 = !gameCopy.playJ1;

    return gameCopy;
}
