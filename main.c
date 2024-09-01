#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"

int main() {
    srand(time(NULL));

    struct Player players[NUM_PLAYERS];
    int order[NUM_PLAYERS];
    int rolls[NUM_PLAYERS];
    int currentPlayer = 0;
    int consecutiveSixes = 0;
    int winner = -1;

    // Initialize players
    initializePlayer(&players[0], "Yellow", 2, 0, 52);
    initializePlayer(&players[1], "Blue", 15, 13, 52);
    initializePlayer(&players[2], "Red", 28, 26, 52);
    initializePlayer(&players[3], "Green", 41, 39, 52);

    // Print the initial setup
    printInitialSetup(players);

    // Determine the player order
    determinePlayerOrder(players, order, rolls);

    // Main game loop
    while (winner == -1) {
        int roll = rollDice();
        printf("\nPlayer %s rolled a %d\n", players[order[currentPlayer]].color, roll);

        if (roll == 6) {
            consecutiveSixes++;
        } else {
            consecutiveSixes = 0;
        }

        if (consecutiveSixes == 3) {
            printf("Player %s rolled three consecutive sixes. Turn skipped.\n", players[order[currentPlayer]].color);
            consecutiveSixes = 0;
            currentPlayer = (currentPlayer + 1) % NUM_PLAYERS;
        } else {
            updatePosition(players, order[currentPlayer], roll);

            // Check if the current player has won
            if (checkWin(&players[order[currentPlayer]])) {
                winner = order[currentPlayer];
            } else if (roll != 6) {
                // Move to the next player if the roll is not a 6
                currentPlayer = (currentPlayer + 1) % NUM_PLAYERS;
            }
        }

        printf("\n");
    }

    printf("Player %s wins the game!\n", players[winner].color);

    return 0;
}
