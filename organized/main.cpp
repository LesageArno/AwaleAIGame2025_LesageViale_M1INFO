#include <iostream>
#include "rules/Rules.h"
#include "players/Player.h"
#include "players/HumanPlayer.h"
// plus tard : #include "players/AlphaBetaPlayer.h"

int main() {

    GameState game;
    initGameState(&game);

    // === Création des joueurs ===
    Player* player1 = new HumanPlayer(true);   // J1
    Player* player2 = new HumanPlayer(false);  // J2
    // exemples futurs :
    // Player* player2 = new AlphaBetaPlayer(false, 6);

    // === Boucle principale ===
    while (true) {

        printBoard(&game);

        Player* currentPlayer = game.playJ1 ? player1 : player2;
        std::string move = currentPlayer->chooseMove(&game);

        // Starvation
        if (move == "-") {
            int captured = starvingCapture(&game);
            std::cout << (game.playJ1 ? "[J1]" : "[J2]")
                      << " is starving, opponent captures "
                      << captured << " seeds.\n";
            game.playJ1 = !game.playJ1;
            continue;
        }

        // Appliquer le coup
        game = Apply(game, move);

        // === Conditions de fin ===
        if (game.countJ1 >= 49) {
            std::cout << "[J1] wins with "
                      << game.countJ1 << " vs "
                      << game.countJ2 << "\n";
            break;
        }

        if (game.countJ2 >= 49) {
            std::cout << "[J2] wins with "
                      << game.countJ2 << " vs "
                      << game.countJ1 << "\n";
            break;
        }

        if (game.countSeed < SEED_LIMIT) {
            if (game.countJ1 > game.countJ2)
                std::cout << "[J1] wins by seed limit\n";
            else if (game.countJ2 > game.countJ1)
                std::cout << "[J2] wins by seed limit\n";
            else
                std::cout << "Draw by seed limit\n";
            break;
        }
    }

    delete player1;
    delete player2;
    return 0;
}
