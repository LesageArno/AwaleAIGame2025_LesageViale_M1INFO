#include <iostream>
#include <algorithm>
#include "rules/Rules.h"
#include "players/Player.h"
#include "players/HumanPlayer.h"
#include "players/MinMaxPlayer.h"
#include "players/AlphaBetaPlayer.h"

/* POUR COMPILER */
//g++ -O2 main_competition.cpp rules/GameState.cpp rules/Rules.cpp players/HumanPlayer.cpp players/MinMaxPlayer.cpp players/AlphaBetaPlayer.cpp players\CommonAI.cpp -o awale_competition_LesageViale.exe

// Maximum number of move
#define MOVELIMIT 400


std::string normalizeMove(const std::string& coup) {
    std::string result = coup;
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    return result;
}

int main() {
    GameState game;
    initGameState(&game);

    bool isJ1 = false; // déterminé par START
    int moveCount = 0;
    bool initialized = false;

    Player* ai = nullptr;

    std::string input;
    std::string lastMove = "-";


    // Lecture des messages venant de l'arbitre
    while (std::getline(std::cin, input)) {
        input = normalizeMove(input);

        // ======================
        // INITIALISATION
        // ======================
        if (!initialized) {
            if (input == "START") {
                // J1
                isJ1 = true;
                ai = new AlphaBetaPlayer(true, 6);
                initialized = true;
            } else {
                //J2
                isJ1 = false;
                ai = new AlphaBetaPlayer(false, 6);
                initialized = true;

                // appliquer le coup adverse (J1)
                game = Apply(game, input);
                moveCount++;
            }
        }
        // ======================
        // COUP ADVERSE NORMAL
        // ======================
        else {
            game = Apply(game, input);
            moveCount++;
        }

        // ======================
        // FIN DE PARTIE ?
        // ======================
        if (game.countJ1 >= 49 || game.countJ2 >= 49) {
            std::cout << "RESULT "
                    << lastMove << " "
                    << game.countJ1 << " "
                    << game.countJ2
                    << std::endl;
            break;
        }

        if (moveCount >= MOVELIMIT) {
            std::cout << "RESULT LIMIT "
                    << game.countJ1 << " "
                    << game.countJ2
                    << std::endl;
            break;
        }

        if (game.countSeed < SEED_LIMIT) {
            std::cout << "RESULT "
                    << lastMove << " "
                    << game.countJ1 << " "
                    << game.countJ2
                    << std::endl;
            break;
        }

        // ======================
        // CALCUL D'UN SEUL COUP IA
        // ======================
        //std::cerr << "[DEBUT] JOUEUR J1 ?? " << game.playJ1 << std::endl;

        std::string move = normalizeMove(ai->chooseMove(&game));
        lastMove = move;
        //std::cerr << "[DEBUT] COUP : "<< move << " | SCORE J1 : " << game.countJ1 << " | SCORE J2 : " << game.countJ2 << std::endl;

        // Starvation
        if (move == "-") {
            starvingCapture(&game);
            game.playJ1 = !game.playJ1;

            std::cout << "-" << std::endl;
            std::cout.flush();
            continue;
        }

        // Appliquer un coup
        game = Apply(game, move);
        moveCount++;

        // Répondre
        std::cout << move << std::endl;
        std::cout.flush();
    }

    delete ai;
    return 0;
}
