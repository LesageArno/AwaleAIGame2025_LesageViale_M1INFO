// g++ .\awaleMinMax.cpp -o .\awaleMinMax.exe to compile

#include <iostream>
#include <string>
#include <vector>
#include <map>

#define BOARDSIZE 16

// Structure definition
struct hole {
    int id;
    int redSeed;
    int blueSeed;
    int transSeed;
};

struct GameState {
    hole board[BOARDSIZE];
    int countJ1;
    int countJ2;
    int countSeed;
    bool playJ1;
};


// Function signatures
void initHole(hole* board);
void initGameState(GameState* game);
void printState(const GameState* game);
void printBoard(const GameState* game);
void play(GameState* game);
int playRed(int moveFrom, bool trans, GameState* game);
int playBlue(int moveFrom, bool trans, GameState* game);
bool checkMoveValidity(int moveFrom, std::string moveSeed, GameState* game);
void capture(int lastMove, GameState* game, bool verbose);
bool checkAvailableMove(GameState* game);
int starvingCapture(GameState* game);
std::vector<std::string> possibleMove(struct GameState* game);
bool isJ1Loosing(struct GameState* game);
bool isJ1Winning(struct GameState* game);
bool isDraw(struct GameState* game);


int main() {
    GameState game;
    initGameState(&game);

    while (true) {
        play(&game);

        if (game.countJ1 >= 49) {
            std::cout << "[J1] Win with " << game.countJ1 << " seeds vs " << game.countJ2 << " seeds for [J2]." << std::endl;
            break;
        } else if (game.countJ2 >= 49) {
            std::cout << "[J2] Win with " << game.countJ2 << " seeds vs " << game.countJ1 << " seeds for [J1]." << std::endl;
            break;
        } else if ((game.countJ1 == 40) && (game.countJ2 == 40)) {
            std::cout << "Draw for [J1] and [J2] with 40 seeds each." << std::endl;
            break;
        } else if (game.countSeed < 10) {
            if (game.countJ1 > game.countJ2) {
                std::cout << "[J1] Win (by 10 seed limit) with " << game.countJ1 << " seeds vs " << game.countJ2 << " seeds for [J2]." << std::endl;
            } else if (game.countJ1 == game.countJ2) {
                std::cout << "Draw (by 10 seed limit) for [J1] and [J2] with " << game.countJ1 << " seeds each." << std::endl;
            } else if (game.countJ1 < game.countJ2) {
                std::cout << "[J2] Win (by 10 seed limit) with " << game.countJ2 << " seeds vs " << game.countJ1 << " seeds for [J1]." << std::endl;
            }
            break;
        }
    }
    return 0;
}



//////// init

void initHole(struct hole* board) {
    for (int i = 0; i<BOARDSIZE; i++) {
        board[i].id = i+1;
        board[i].redSeed = 2;
        board[i].blueSeed = 2;
        board[i].transSeed = 2;
    }
}
void initGameState(GameState* game) {
    initHole(game->board);

    game->countJ1 = 0;
    game->countJ2 = 0;
    game->countSeed = 3 * 2 * BOARDSIZE;
    game->playJ1 = true;
}



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

// Play method
void play(struct GameState* game) {
    int moveFrom;
    std::string moveSeed;
    
    // Select moves for J1 if selected else J2
    if (game->playJ1) {
        
        // If no move, then starving state and end of the game
        if (!checkAvailableMove(game)) {
            std::cout << "[J1] is starving, [J2] takes " << starvingCapture(game) << " seeds from its holes." << std::endl;
            game->playJ1 = !game->playJ1;
            return;
        }

        while (true) {
            // Print board
            printBoard(game);

            // Ask move
            std::cout << "[J1] You can move from holes: 1, 3, 5, 7, 9, 11, 13, 15 and select move from r, b, tr, tb (format int str): ";
            std::cin >> moveFrom >> moveSeed;

            // Check move
            if (checkMoveValidity(moveFrom, moveSeed, game)) {
                std::cout << "[J1] succesfully played: " << moveFrom << moveSeed << std::endl;
                break;
            } else {
                std::cout << "[J1] Invalid move: " << moveFrom << moveSeed << std::endl;
            }
        }
    } else {

        // If no move, then starving state and end of the game
        if (!checkAvailableMove(game)) {
            std::cout << "[J2] is starving, [J1] takes " << starvingCapture(game) << " seeds from its holes." << std::endl;
            game->playJ1 = !game->playJ1;
            return;
        }

        while (true) {
            printBoard(game);
            std::cout << "[J2] You can move from holes: 2, 4, 6, 8, 10, 12, 14, 16 and select move from r, b, tr, tb (format int str): ";
            std::cin >> moveFrom >> moveSeed;
            if (checkMoveValidity(moveFrom, moveSeed, game)) {
                std::cout << "[J2] succesfully played: " << moveFrom << moveSeed << std::endl;
                break;
            } else {
                std::cout << "[J2] Invalid move: " << moveFrom << moveSeed << std::endl;
            }
        }
    }

    // Play the move and update the board
    int lastMove;
    if (moveSeed == "b") {
        lastMove = playBlue(moveFrom, false, game);
    } else if (moveSeed == "tb") {
        lastMove = playBlue(moveFrom, true, game);
    } else if (moveSeed == "r") {
        lastMove = playRed(moveFrom, false, game);
    } else if (moveSeed == "tr") {
        lastMove = playRed(moveFrom, true, game);
    }

    // Capture if possible
    capture(lastMove, game, true);
    
    game->playJ1 = !game->playJ1;
}

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













































////////////////////////////////////////////////////////////////////////// MIN-MAX
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
    // If more than 40 seed recolted each, then draw
    if ((game->countJ1 >= 40) && (game->countJ2 >= 40)) {
        return true;
    }

    // If J1 equal J2 the majority of the seed when less than 10 seed, loose
    if ((game->countSeed < 10) && (game->countJ1 == game->countJ2)) {
        return true;
    }

    // Else, not draw
    return false;
}

GameState Apply(GameState game, std::string move) {
    GameState gameCopy = game;

    // If no move available, then starving
    if (move == "-") {
        starvingCapture(&gameCopy);
        gameCopy.playJ1 = !gameCopy.playJ1;
        return gameCopy;
    }

    // Process the move
    int sep = move.find(' ');
    std::string moveColour = move.substr(sep+1);
    int moveFrom = std::stoi(move.substr(0, sep));

    if (moveColour == "b") {
        moveFrom = playBlue(moveFrom, false, &gameCopy);
    } else if (moveColour == "tb") {
        moveFrom = playBlue(moveFrom, true, &gameCopy);
    } else if (moveColour == "r") {
        moveFrom = playRed(moveFrom, false, &gameCopy);
    } else if (moveColour == "tr") {
        moveFrom = playRed(moveFrom, true, &gameCopy);
    }

    // Capture if possible
    capture(moveFrom, &gameCopy, false);
    
    gameCopy.playJ1 = !gameCopy.playJ1;

    return gameCopy;

}

float evaluate(GameState* game);
int potentialCaptures(struct GameState* game);
/*
float evaluate(GameState* game) {
    return 0.;
}
*/
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



int potentialCaptures (struct GameState* game){
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


float MinMax(GameState game, bool isMax, int pmax) {
    GameState gameCopy = game;

    if (isJ1Winning(&gameCopy)) {return -1.;}
    if (isJ1Loosing(&gameCopy)) {return 1.;}
    if (isDraw(&gameCopy)) {return 0.;}
    if (pmax == 0) {return evaluate(&gameCopy);}

    float minVal = 2.;
    float maxVal = -2.;
    float val;

    if (!checkAvailableMove(&gameCopy)) {
        return MinMax(Apply(gameCopy, "-"), !isMax, pmax-1);
    }

    for (std::string move : possibleMove(&gameCopy)) {
        val = MinMax(Apply(gameCopy, move), !isMax, pmax-1);
        if (val < minVal) {
            minVal = val;
        }

        if (val > maxVal) {
            maxVal = val;
        }
    }

    if (isMax) {return maxVal;}
    if (!isMax) {return minVal;}
}

std::string DecisionMinMax(GameState* game, int pmax) {
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
        val = MinMax(Apply(*game, move), false, pmax);
        if (val > bestVal) {
            bestVal = val;
            bestMove = move;
        }
    }

    return bestMove;
}