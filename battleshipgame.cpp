// By: mathewn24
#include <iostream>
#include "battleshipboard.h"
#include <cctype>

int main() {
    // prompt user for seed, game mode, ships setup before gameplay
    unsigned int seed;
    bool gameMode = false;
    char mode;
    bool manualSetup = false;
    char manual;

    std::cout << "Enter seed: ";
    std::cin >> seed;
    std::srand(seed); // Set seed value for pseudo-random number generation.
    std::cout << "Play in game mode (Y/N)? ";
    std::cin >> mode;
    std::cout << "Manually set up ships (Y/N)? ";
    std::cin >> manual;
    mode = std::toupper(mode);
    manual = std::toupper(manual);
    
    if (mode == 'Y')
    { gameMode = true; } 
    else
    { gameMode = false; }
    // If false, it is in debug mode.

    if (manual == 'Y')
    { manualSetup = true; }
    else
    { manualSetup = false; }
    
    // initialize boards and fleets
    char myBoard[N][N] = { ' ' }, enBoard[N][N] = { ' ' };
    Fleet myFleet = { "Your", {5, 4, 3, 3, 2} };
    Fleet enFleet = { "Enemy", {5, 4, 3, 3, 2} };

    std::fill(myBoard[0] + 0, myBoard[N-1] + N, BLANK);
    std::fill(enBoard[0] + 0, enBoard[N-1] + N, BLANK);
    
    if (manualSetup) {
        printBoards(enBoard, myBoard, gameMode);
        manuallyPlaceShips(myBoard);
    }
    else {
        randomlyPlaceShips(myBoard);
    }
    randomlyPlaceShips(enBoard);

    bool gameOver = false;
    std::string winner = "";
    Fleet *attacker = &myFleet, *opponent = &enFleet, *tmp = NULL;

    // start the game loop
    for (int round = 1; !gameOver; round++) {
        std::cout << "Round " << round << ":" << std::endl;
        printBoards(enBoard, myBoard, gameMode);
        printStatus(enFleet, myFleet);
        
        // get shoot location
        int x, y;
        while (true) {
            bool ok;
            if (attacker == &myFleet) {
                ok = getHumanMove(y, x) && isValidShoot(enBoard, y, x);
                if (!ok) 
                    std::cout << "Invalid shot location!" << std::endl;
            }
            else {
                ok = getComputerMove(myBoard, y, x) && isValidShoot(myBoard, y, x);
            }
            if (ok) 
                break;
        }

        // carry out the shoot
        bool hit = shoot(attacker == &myFleet ? enBoard : myBoard, y, x, opponent);
        if (hit) {
            // check if all opponent ships have sunk
            // (if so, set winner and game over flag to leave the loop)s
            int count = 0;
            for (int i = 0; i < 5; i++) {
                if (opponent -> alive[i] <= 0)
                { count++; }
            }
            if (count >= 5) {
                winner = attacker -> name;
                gameOver = true;
            }
        }

        // swap the player's turn
        tmp = attacker;
        attacker = opponent;
        opponent = tmp;
    }

    // print final gameboards, game over message and the winner
    gameMode = false;
    printBoards(enBoard, myBoard, gameMode);
    printStatus(enFleet, myFleet);
    std::cout << "Game over!" << std::endl;
    std::cout << winner << " fleet wins!" << std::endl;

    return 0;
}