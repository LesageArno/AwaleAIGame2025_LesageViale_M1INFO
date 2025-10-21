#include <stdio.h>
#include <stdlib.h>

#define PLATESIZE 16

struct hole {
    int id;
    int redSeed;
    int blueSeed;
    int transSeed;
};

int countJ1 = 0;
int countJ2 = 0;

void initHole(struct hole* plate) {
    for (int i = 0; i<PLATESIZE; i++) {
        plate[i].id = i+1;
        plate[i].redSeed = 2;
        plate[i].blueSeed = 2;
        plate[i].transSeed = 2;
    }
}

void printState(struct hole* plate) {
    for (int i = 0; i<PLATESIZE; i++) {
        printf("id: %d, redSeed: %d, blueSeed: %d, transSeed: %d\n", plate[i].id, plate[i].redSeed, plate[i].blueSeed, plate[i].transSeed);
    }
}

int main() {
    struct hole plate[PLATESIZE];
    initHole(plate);
    printState(plate);

    return 0;
}