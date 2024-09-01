# Ludo Game

This is a simple command-line implementation of the classic board game Ludo, written in C. The game supports up to four players and allows for basic gameplay mechanics, including rolling dice and moving pieces.

## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Compilation](#compilation)
- [Usage](#usage)
- [Game Rules](#game-rules)
- [Credits](#credits)

## Features

- Simulates a basic Ludo game with up to 4 players.
- Players roll dice to move their pieces around the board.
- The game determines the winner once a player successfully moves all their pieces to the home position.
- Implemented in C for educational purposes.

## Requirements

- GCC (GNU Compiler Collection) or any compatible C compiler.
- Unix-based system (Linux, macOS) or Windows with a compatible shell environment like WSL or Cygwin.

## Installation

1. Clone the repository to your local machine:

    ```bash
    git clone https://github.com/yourusername/ludo-game.git
    cd ludo-game
    ```

2. Ensure you have GCC installed. You can install GCC on Ubuntu or Debian-based systems using:

    ```bash
    sudo apt-get update
    sudo apt-get install gcc
    ```

## Compilation

To compile the Ludo game, run the following command in your terminal:

```bash
gcc main.c logic.c -o ludo
```

This command compiles the `main.c` and `logic.c` files and creates an executable named `ludo`.

## Usage

To play the game, run the executable in your terminal:

```bash
./ludo
```
