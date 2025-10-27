// g++ .\awale.cpp -o .\awale.exe to compile

#include <iostream>
#include <string>

#define BOARDSIZE 16

// Structure definition
struct hole {
    int id;
    int redSeed;
    int blueSeed;
    int transSeed;
};

// Total for each player
int countJ1 = 0;
int countJ2 = 0;

// Player turn
bool playJ1 = true;

// Function signatures
void initHole(struct hole* board);
void printState(struct hole* board);
void printBoard(struct hole* board);
void play(struct hole* board);
int playRed(int moveFrom, bool trans, struct hole* board);
int playBlue(int moveFrom, bool trans, struct hole* board);
bool checkMoveValidity(int moveFrom, std::string moveSeed, struct hole* board);
void capture();





int main() {
    struct hole board[BOARDSIZE];
    initHole(board);
    while (true) {
        play(board);
    }

    return 0;
}










// Board initialisation method
void initHole(struct hole* board) {
    for (int i = 0; i<BOARDSIZE; i++) {
        board[i].id = i+1;
        board[i].redSeed = 2;
        board[i].blueSeed = 2;
        board[i].transSeed = 2;
    }
}

// Board representation method
void printBoard(struct hole* board) {
    std::cout << "|--------|--------|--------|--------|--------|--------|--------|--------|" << std::endl;

    for (int i = 0; i<BOARDSIZE/2; i++) {
        printf("|%02d\033[41m%02d\033[44m%02d\033[43m%02d\033[0m", i+1, board[i].redSeed, board[i].blueSeed, board[i].transSeed);
    }
    std::cout << "|" << std::endl << "|--------|--------|--------|--------|--------|--------|--------|--------|" << std::endl;
    for (int i = BOARDSIZE-1; i>=BOARDSIZE/2; i--) {
        printf("|%02d\033[41m%02d\033[44m%02d\033[43m%02d\033[0m", i+1, board[i].redSeed, board[i].blueSeed, board[i].transSeed);
    }
    std::cout << "|" << std::endl << "|--------|--------|--------|--------|--------|--------|--------|--------|" << std::endl;
}

// State representation method
void printState(struct hole* board) {
    for (int i = 0; i<BOARDSIZE; i++) {
        std::cout << "id: " << board[i].id << ", redSeed: " << board[i].redSeed << ", blueSeed: " << board[i].blueSeed << ", transSeed: " << board[i].transSeed << std::endl;
    }
}

// Play method
void play(struct hole* board) {
    int moveFrom;
    std::string moveSeed;
    
    // Select moves for J1 if selected else J2
    if (playJ1) {
        while (true) {
            // Print board
            printBoard(board);

            // Ask move
            std::cout << "[J1] You can move from holes: 1, 3, 5, 7, 9, 11, 13, 15 and select move from r, b, tr, tb (format int str): ";
            std::cin >> moveFrom >> moveSeed;

            // Check move
            if (checkMoveValidity(moveFrom, moveSeed, board)) {
                std::cout << "[J1] succesfully played: " << moveFrom << moveSeed << std::endl;
                break;
            } else {
                std::cout << "[J1] Invalid move: " << moveFrom << moveSeed << std::endl;
            }
        }
    } else {
        while (true) {
            printBoard(board);
            std::cout << "[J2] You can move from holes: 2, 4, 6, 8, 10, 12, 14, 16 and select move from r, b, tr, tb (format int str): ";
            std::cin >> moveFrom >> moveSeed;
            if (checkMoveValidity(moveFrom, moveSeed, board)) {
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
        lastMove = playBlue(moveFrom, false, board);
    } else if (moveSeed == "tb") {
        lastMove = playBlue(moveFrom, true, board);
    } else if (moveSeed == "r") {
        lastMove = playRed(moveFrom, false, board);
    } else if (moveSeed == "tr") {
        lastMove = playRed(moveFrom, true, board);
    }
    
    playJ1 = !playJ1;
}

// Play red function
int playRed(int moveFrom, bool trans, struct hole* board) {
    // Add red seed to the sum
    int redPlace = board[moveFrom-1].redSeed;
    board[moveFrom-1].redSeed = 0;

    // Add transparent seed to the sum if requested
    int transPlace = 0;
    if (trans) {
        transPlace = board[moveFrom-1].transSeed;
        board[moveFrom-1].transSeed = 0;
    }

    // Initialise current position to next cell mod 16
    int currentPosition = moveFrom%BOARDSIZE;

    // Loop until no more seed are to place
    while ((transPlace > 0) || (redPlace > 0)) {
        // Place transparent first then red
        if (transPlace > 0) {
            transPlace--;
            board[currentPosition].transSeed = board[currentPosition].transSeed + 1;
        } else if (redPlace > 0) {
            redPlace--;
            board[currentPosition].redSeed = board[currentPosition].redSeed + 1;
        }

        // Go to next hole
        currentPosition = (currentPosition + 1)%BOARDSIZE;
    }
    // Return last position
    return currentPosition;
}

// Play blue function
int playBlue(int moveFrom, bool trans, struct hole* board) {
    // Add blue seed to the sum
    int bluePlace = board[moveFrom-1].blueSeed;
    board[moveFrom-1].blueSeed= 0;

    // Add transparent seed to the sum if requested
    int transPlace = 0;
    if (trans) {
        transPlace = board[moveFrom-1].transSeed;
        board[moveFrom-1].transSeed = 0;
    }

    // Initialise current position to next cell mod 16
    int currentPosition = moveFrom%BOARDSIZE;

    // Loop until no more seed are to place
    while ((transPlace > 0) || (bluePlace > 0)) {
        // Place transparent first then blue
        if (transPlace > 0) {
            transPlace--;
            board[currentPosition].transSeed = board[currentPosition].transSeed + 1;
        } else if (bluePlace > 0) {
            bluePlace--;
            board[currentPosition].blueSeed = board[currentPosition].blueSeed + 1;
        }
        // Go to next opponent position
        currentPosition = (currentPosition + 2)%BOARDSIZE;
    }

    // Return last position
    return currentPosition;
}

// Check move validity function
bool checkMoveValidity(int moveFrom, std::string moveSeed, struct hole* board) {
    // Check for illegal move (move outside the board)
    if ((moveFrom > BOARDSIZE) || (moveFrom < 1)) {
        return false;
    }

    // Check for illegal move (move from opponent position) 
    if ((playJ1 && (moveFrom%2 == 0))||(!playJ1 && (moveFrom%2 == 1))) {
        return false;
    }

    // Check if redSeed are remaining in the hole
    if (moveSeed=="r") {
        if (board[moveFrom-1].redSeed==0) {
            return false;
        }
        return true;
    }

    // Check if blueSeed are remaining in the hole
    if (moveSeed=="b") {
        if (board[moveFrom-1].blueSeed==0) {
            return false;
        }
        return true;
    }

    // Check if transSeed are remaining in the hole
    if ((moveSeed=="tr") || (moveSeed=="tb")) {
        if (board[moveFrom-1].transSeed==0) {
            return false;
        }
        return true;
    }

    // Illegal seed
    return false;
}