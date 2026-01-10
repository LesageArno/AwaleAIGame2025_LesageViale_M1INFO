#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "rules/Rules.h"
#include "players/Player.h"
#include "players/HumanPlayer.h"
#include "players/MinMaxPlayer.h"
#include "players/AlphaBetaPlayer.h"

/* POUR COMPILER */
//g++ -O2 mainCompet.cpp rules/GameState.cpp rules/Rules.cpp players/HumanPlayer.cpp players/MinMaxPlayer.cpp players/AlphaBetaPlayer.cpp players\CommonAI.cpp -o awale_compet.exe

// Maximum number of move (to implement)
#define MOVELIMIT 400

std::string normalizeMove(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
            out += c;
        }
    }
    return out;
}


int main() {

    GameState game;
    initGameState(&game);

    Player* me = nullptr;
    bool initialized = false;

    std::string etat;

    while (std::getline(std::cin, etat)) {

        // ===== Initialisation dynamique J1/J2 =====
        if (!initialized) {
            bool isJ1 = (etat == "START");
            me = new AlphaBetaPlayer(isJ1, 2); //6 POUR COMPET
            initialized = true;
        }

        // ===== Réception =====
        if (etat == "START") {
            // rien à appliquer
        }
        else if (etat == "END" || etat.rfind("RESULT", 0) == 0) {
            break;
        }
        else {
            // Coup adverse
            game = Apply(game, etat);
        }

        // ===== Calcul du coup =====
        std::string coup = me->chooseMove(&game);
        //coup = normalizeMove(coup);

        // ===== Envoi =====
        std::cout << coup << std::endl;
        std::cout.flush();

        // ===== Mise à jour locale =====
        game = Apply(game, coup);
    }

    delete me;
    return 0;
}