#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BOARD_SIZE 52
#define HOME_PATH_SIZE 5
#define NUM_PLAYERS 4
#define PIECES_PER_PLAYER 4

// Structure for a piece
struct Piece {
    int position; // -1: at base, 0-51: board positions, 52-57: home path positions, 58: final position
    char name[3];
};

// Structure for a player
struct Player {
    char color[10];
    struct Piece pieces[PIECES_PER_PLAYER];
    int startPosition;      // Starting point for each player on the board
    int approachPosition;   // Position before entering the home path
    int homePathStart;      // Home path entry position
};

// Function to initialize player pieces and starting points
void initializePlayer(struct Player *player, const char *color, int startPosition, int approachPosition, int homePathStart) {
    strcpy(player->color, color);
    player->startPosition = startPosition;
    player->approachPosition = approachPosition;
    player->homePathStart = homePathStart;
    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        player->pieces[i].position = -1; // -1 indicates the piece is at base
        player->pieces[i].name[0] = color[0]; // First letter of the color
        player->pieces[i].name[1] = '1' + i; // Number of the piece
        player->pieces[i].name[2] = '\0';
    }
}

// Function to roll the dice
int rollDice() {
    return (rand() % 6) + 1;
}

// Function to count pieces on the board and at the base
void countPieces(struct Player *player, int *onBoard, int *atBase) {
    *onBoard = 0;
    *atBase = 0;
    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (player->pieces[i].position == -1) {
            (*atBase)++;
        } else if (player->pieces[i].position >= 0 && player->pieces[i].position < BOARD_SIZE + HOME_PATH_SIZE + 1) {
            (*onBoard)++;
        }
    }
}

// Function to check if a piece is blocking another
int isBlocking(struct Player *players, int currentPlayer, int newPosition) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (i != currentPlayer) {
            for (int j = 0; j < PIECES_PER_PLAYER; j++) {
                if (players[i].pieces[j].position == newPosition) {
                    return i; // Returns the index of the player being blocked
                }
            }
        }
    }
    return -1; // No blocking
}

// Function to convert position to square ID or home path ID
void getSquareID(struct Player *players, int playerIndex, int position, char* result) {
    if (position >= 0 && position < BOARD_SIZE) {
        sprintf(result, "L%d", position);
    } else if (position >= BOARD_SIZE && position < BOARD_SIZE + HOME_PATH_SIZE) {
        int homePathCell = position - BOARD_SIZE;
        sprintf(result, "%shomepath%d", players[playerIndex].color, homePathCell); // Use the player's color
    } else if (position == BOARD_SIZE + HOME_PATH_SIZE + 1) {
        sprintf(result, "Final");
    }
}


// Function to update player position and output required messages
void updatePosition(struct Player *players, int currentPlayer, int roll) {
    struct Player *player = &players[currentPlayer];
    int onBoard = 0, atBase = 0;
    int validMove = 0;

    countPieces(player, &onBoard, &atBase);

    // If there's a piece at the base and the roll is 6, move it to the starting point
    if (roll == 6 && atBase > 0) {
        for (int i = 0; i < PIECES_PER_PLAYER; i++) {
            if (player->pieces[i].position == -1) { // Find the piece at the base
                player->pieces[i].position = player->startPosition;
                char startID[20];
                getSquareID(players, currentPlayer, player->pieces[i].position, startID);
                printf("%s player moves piece %s to the starting point (%s).\n", player->color, player->pieces[i].name, startID);
                validMove = 1;
                break;
            }
        }
    }

    // Move the piece on the board or home path
    if (!validMove) {
        for (int i = 0; i < PIECES_PER_PLAYER; i++) {
            int pos = player->pieces[i].position;
            char oldID[20], newID[20];

            // If the piece is on the board
            if (pos >= 0 && pos < BOARD_SIZE) {
                int newPosition = pos + roll;

                // Calculate the position considering the circular board
                if (newPosition >= BOARD_SIZE) {
                    newPosition -= BOARD_SIZE;
                }

                // Check if the new position is already occupied by another piece of the same player
                int ownPieceBlocking = 0;
                for (int j = 0; j < PIECES_PER_PLAYER; j++) {
                    if (i != j && player->pieces[j].position == newPosition) {
                        ownPieceBlocking = 1;
                        break;
                    }
                }

                if (ownPieceBlocking) {
                    getSquareID(players, currentPlayer, pos, oldID);
                    printf("%s player cannot move %s to %s because it's occupied by another of their pieces.\n",
                           player->color, player->pieces[i].name, oldID);
                    continue;
                }

                // If the piece reaches the approach position and can enter the home path
                if (pos <= player->approachPosition && newPosition > player->approachPosition) {
                    int homePathPosition = newPosition - player->approachPosition;
                    if (homePathPosition <= HOME_PATH_SIZE) {
                        player->pieces[i].position = BOARD_SIZE + homePathPosition;
                        getSquareID(players, currentPlayer, player->pieces[i].position, newID);
                        printf("%s player moved %s to home path %s\n", player->color, player->pieces[i].name, newID);
                        validMove = 1;
                        break;
                    } else {
                        getSquareID(players, currentPlayer, player->pieces[i].position, oldID);
                        printf("%s player cannot move %s to home path beyond %s.\n", player->color, player->pieces[i].name, oldID);
                    }
                }

                // Regular movement on the board
                else if (newPosition < BOARD_SIZE) {
                    player->pieces[i].position = newPosition;
                    int blockedPlayer = isBlocking(players, currentPlayer, newPosition);
                    if (blockedPlayer != -1) {
                        getSquareID(players, currentPlayer, newPosition, oldID);
                        printf("%s player landed on %s player's piece at %s and sent it back to the base!\n",
                               player->color, players[blockedPlayer].color, oldID);
                        for (int j = 0; j < PIECES_PER_PLAYER; j++) {
                            if (players[blockedPlayer].pieces[j].position == newPosition) {
                                players[blockedPlayer].pieces[j].position = -1; // Send the blocked piece back to the base
                                break;
                            }
                        }
                    }
                    getSquareID(players, currentPlayer, newPosition, newID);
                    printf("%s player moved %s to position %s\n", player->color, player->pieces[i].name, newID);
                    validMove = 1;
                    break;
                }
            }

            // If the piece is on the home path
            else if (pos >= BOARD_SIZE && pos < BOARD_SIZE + HOME_PATH_SIZE) {
                int newHomePosition = pos + roll;

                // Check if piece moves to final position
                if (newHomePosition == BOARD_SIZE + HOME_PATH_SIZE + 1) {
                    player->pieces[i].position = BOARD_SIZE + HOME_PATH_SIZE + 1;
                    getSquareID(players, currentPlayer, player->pieces[i].position, newID);
                    printf("%s player moved %s to the final position (%s).\n", player->color, player->pieces[i].name, newID);
                    validMove = 1;
                    break;
                }
                // Regular movement on the home path
                else if (newHomePosition < BOARD_SIZE + HOME_PATH_SIZE) {
                    player->pieces[i].position = newHomePosition;
                    getSquareID(players, currentPlayer, newHomePosition, newID);
                    printf("%s player moved %s to home path %s\n", player->color, player->pieces[i].name, newID);
                    validMove = 1;
                    break;
                }
                // If move exceeds final position
                else {
                    getSquareID(players, currentPlayer, pos, oldID);
                    printf("%s player cannot move %s beyond %s.\n", player->color, player->pieces[i].name, oldID);
                }
            }
        }
    }
    
    // Print the updated status of pieces on the board and at the base
    if (validMove || roll == 6) {
        countPieces(player, &onBoard, &atBase);
        printf("[%s] player now has %d/%d pieces on the board and %d/%d pieces on the base.\n",
               player->color, onBoard, PIECES_PER_PLAYER, atBase, PIECES_PER_PLAYER);
    }

    if (!validMove && roll == 6) {
        printf("%s player rolled a 6 but couldn't move any pieces. An extra turn is granted.\n", player->color);
    }
}



// Function to check if a player has all pieces in the final position
// Function to check if a player has all pieces in the final position
int checkWin(struct Player *player) {
    int count = 0;
    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (player->pieces[i].position == BOARD_SIZE + HOME_PATH_SIZE + 1) {
            count++;
        }
    }
    return (count == PIECES_PER_PLAYER);
}


// Function to determine the first player based on dice rolls and print the player order
void determinePlayerOrder(struct Player *players, int *order, int *rolls) {
   
    for (int i = 0; i < NUM_PLAYERS; i++) {
        rolls[i] = rollDice();
        printf("%s player rolled a %d\n", players[i].color, rolls[i]);
        order[i] = i;
    }

    // Sorting players based on dice rolls
    for (int i = 0; i < NUM_PLAYERS - 1; i++) {
        for (int j = i + 1; j < NUM_PLAYERS; j++) {
            if (rolls[i] < rolls[j]) {
                int temp = rolls[i];
                rolls[i] = rolls[j];
                rolls[j] = temp;

                temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }

    // Find the player with the highest roll
    int highestRollPlayer = order[0];
    printf("\n%s player has the highest roll and will begin the game.\n", players[highestRollPlayer].color);

    // Print the player order in the new format
    printf("The order of a single round is %s, %s, %s, and %s.\n",
           players[order[0]].color,
           players[order[1]].color,
           players[order[2]].color,
           players[order[3]].color);
}


// Function to print the initial game setup
void printInitialSetup(struct Player *players) {
    printf("Welcome to Ludo Game!\n\n");
    for (int i = 0; i < NUM_PLAYERS; i++) {
        printf("Player %s pieces: ", players[i].color);
        for (int j = 0; j < PIECES_PER_PLAYER; j++) {
            printf("%s", players[i].pieces[j].name);
            if (j < PIECES_PER_PLAYER - 1) {
                printf(", ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

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

