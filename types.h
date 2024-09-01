#ifndef TYPES_H
#define TYPES_H

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

// Function declarations
void initializePlayer(struct Player *player, const char *color, int startPosition, int approachPosition, int homePathStart);
int rollDice();
void countPieces(struct Player *player, int *onBoard, int *atBase);
int isBlocking(struct Player *players, int currentPlayer, int newPosition);
void getSquareID(struct Player *players, int playerIndex, int position, char* result);
void updatePosition(struct Player *players, int currentPlayer, int roll);
int checkWin(struct Player *player);
void determinePlayerOrder(struct Player *players, int *order, int *rolls);
void printInitialSetup(struct Player *players);

#endif // TYPES_H
