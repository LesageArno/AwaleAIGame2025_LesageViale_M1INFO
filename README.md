# Projet AI Game Programming 2025-2026 : Awalé
## Auteurs : Arno LESAGE, Jean-Jacques VIALE

Pour des informations sur les choix d'implémentations et la présentation du jeu de l'Awalé, veuillez trouver le rapport dans <a href="report/Rapport AI Game Programming LESAGE VIALE M1 Informatique 2025-2026.pdf">report/Rapport AI Game Programming LESAGE VIALE M1 Informatique 2025-2026.pdf</a>.

Ce `README` traite des informations pratiques pour l'exécution du code.

**Important :** Le code adaptant notre implémentation de l'Awalé pour la compétition automatique ([Arbitre.java](Arbitre.java), [organized/awale_competition_LesageViale.exe](organized/awale_competition_LesageViale.exe) et [organized/main_competition.cpp](organized/main_competition.cpp)) est pour le moment bugué rendant la compétition automatique non représentatif de notre travail. La compétition manuelle est néanmoins toujours possible via [organized/awale.exe](organized/awale.exe) et [organized/main.cpp](organized/main.cpp).

### Descriptif de l'architecture

Au niveau de l'organisation du code, `organized/players` contiens tous le code `C++` relatif à la gestions des différents types d'agents (MinMax, humain ou AlphaBeta). Le fichier `organized/CommonAI.cpp` contient des fonctions utilisés dans les deux implémentations MinMax et AlphaBeta. 

#### Modifier la fonction d'évaluation utilisée
- Le fichier `organized/CommonAI.h` est tout aussi important, car c'est dans celui-ci que l'on défini quelles fonctions d'évaluation sont utilisées par les joueurs (`J1_EVALUATION_FUNC` et `J1_EVALUATION_FUNC`).

#### Modifier les paramètres de profondeur adaptative
- Pour modifier les paramètres de profondeur adaptative du modèle avec coupes Alpha-Beta, la fonction `findBestPmax` du fichier `organized/AlphaBetaPlayer.cpp` est facilement modifiable à ces fins.

Ensuite, le répertoire `organized/rules` contient un fichier correspondant à la structure de donnée représentant le jeu ([organized/rules/GameState.cpp](organized/rules/GameState.cpp), [organized/rules/GameState.cpp](organized/rules/GameState.h)), mais aussi un le code gérant l'entièreté de la logique du jeu ([organized/rules/Rules.cpp](organized/rules/Rules.cpp), [organized/rules/Rules.h](organized/rules/Rules.h)).

#### Définir la profondeur utilisé et le type d'agent
- Enfin, le fichier [organized/main.cpp](organized/main.cpp) est celui mettant en relation les deux joueurs. Celui-ci permet de choisir le type de joueur (premières lignes du main), mais aussi de définir la profondeur à utiliser (-1 pour adaptatif, une valeur positive pour une profondeur fixe).

#### Compilation
Depuis le répertoire `organized`, exécutez la commande suivante dans le shell :
```
# Pour la compétition manuelle
g++ -O2 main.cpp rules/GameState.cpp rules/Rules.cpp players/HumanPlayer.cpp players/MinMaxPlayer.cpp players/AlphaBetaPlayer.cpp players\CommonAI.cpp -o awale.exe

# Pour la compétition automatique
g++ -O2 main_competition.cpp rules/GameState.cpp rules/Rules.cpp players/HumanPlayer.cpp players/MinMaxPlayer.cpp players/AlphaBetaPlayer.cpp players\CommonAI.cpp -o awale_competition_LesageViale.exe
```
