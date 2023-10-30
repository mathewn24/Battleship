// Name:       Mathews Noel
// Student ID: 1155130840
// Email:      1155130840@link.cuhk.edu.hk

#include <iostream>
#include <iomanip>
#include <cctype>
#include "battleshipboard.h"

// Return a 5-letter string showing which ships of the specified fleet have sunk
std::string fleetStatus(Fleet fleet) {
    std::string status = "";
    for (int i = 0; i < 5; i++) {
        if (fleet.alive[i] > 0) {
            status += SHIP_TYPE[i][0];
        } else if (fleet.alive[i] <= 0)
        { status += HIT; }
    }    
    return status;
}

// Print the status string of each fleet
void printStatus(Fleet fleet1, Fleet fleet2) {
    std::cout << std::setw(N*2 + 2) << std::left;
    std::cout << fleet1.name + " Fleet: " + fleetStatus(fleet1) << "  ";
    std::cout << std::setw(N*2 + 2) << std::left;
    std::cout << fleet2.name + " Fleet: " + fleetStatus(fleet2) << std::endl;
}

// Print the user and computer game boards side by side
void printBoards(char board1[][N], char board2[][N], bool gameMode) {
    std::cout << std::setw(N*2 + 2) << std::left << "Enemy Ocean" << "  "
         << std::setw(N*2 + 2) << std::left << "Your Ocean" << std::endl << std::right;
    
    // print column headers for both game boards
    for (int i = 0; i < 2; i++) {
        std::cout << string(2 + i*2, ' ');
        for (int j = 0; j < N; j++)
            std::cout << std::setw(2) << char('A' + j);
    }
    std::cout << std::endl;

    // print row indexes and board content for both game boards
    for (int k = 0; k < N; k++) {   //Prints row index for enemy board.
        std::cout << std::setw(2) << k+1;
        for (int l = 0; l < 2*N; l++) {
            if (l == N) {   //Prints row index for second board.
                std::cout << "  " << std::setw(2) << k+1;
            }
            if (l >= 0 && l < N) { 
                if (gameMode == false) // Only show Enemy Ocean if gameMode is false.
                { std::cout << std::setw(2) << board1[k][l]; }
                // If the enemy position has not been attacked before then display BLANK.
                else if (gameMode == true) {
                        if ((board1[k][l] == HIT) || (board1[k][l] == MISS)) {
                            std::cout << std::setw(2) << board1[k][l];
                        } else {
                            std::cout << std::setw(2) << BLANK;
                        }
                    }
            }
            if (l >= N && l < 2*N)
            { std::cout << std::setw(2) << board2[k][l-N]; } 
        }
        std::cout << std::endl;
    }
}

// Check if the specified location (y, x) is within the board
bool isValidCell(int y, int x) {
    if (y > N-1 || x > N-1 || y < 0 || x < 0)
    { return false; }
    return true;
}

// Check if a shoot location (y, x) is valid
bool isValidShoot(char board[][N], int y, int x) {
    bool boundary;
    boundary = isValidCell(y, x);
    if (boundary == true && (board[y][x] != HIT) && (board[y][x] != MISS))
    { return true; }
    return false;
}

// Convert ship letter to its index in the ship type array
int indexOf(char ship) {
    for (int i = 0; i < 5; i++) {
        if (SHIP_TYPE[i][0] == ship)
        { return i; }
    }
    return (-1);
}

// Carry out a shot on the specified board at location (y, x)
bool shoot(char board[][N], int y, int x, Fleet* fleet) {
    int index;
    if (board[y][x] != BLANK) {
        //Store the index of the specified fleet.
        index = indexOf(board[y][x]);
        board[y][x] = HIT; // Mark position as hit.
        fleet -> alive[index] -= 1; // Subtract corresponding ship (stored in index) health by 1;
        if (fleet -> alive[index] <= 0)
        { std::cout << fleet -> name << " " << SHIP_TYPE[index] << " sank!" << std::endl; }    
    }
    else if (board[y][x] == BLANK) { 
        board[y][x] = MISS;
        return false;
    }

    return true;
}

// Get location (y, x) from the user via console input
bool getCellFromConsole(int& y, int& x) {
    char col; int row;
    std::cin >> col >> row;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }
    col = std::toupper(col); // Convert char to uppercase if it is lowercase.
    x = int(col - 'A'); // Subtract char A from answer to get distance and covert it to int.
    y = row - 1; // Subtract row by 1 since array index is from 0 to N-1.
    return true;
}

// Get a random location (y, x) within board size
void getRandomCell(int& y, int& x) {
    y = rand() % N;
    x = rand() % N;
}

// Prompt the user for shoot location (y, x)
bool getHumanMove(int& y, int& x) {
    std::cout << "Enter attack location: ";
    return getCellFromConsole(y, x);
}

// Get shoot location (y, x) by the computer player's strategy
bool getComputerMove(char board[][N], int& y, int& x) {
    // try to shoot neighborhood of an existing hit first
    const int TRIALS = 1000;
    int dy[] = { -1,  0,  1,  0 };  // deltas for getting the cell at 
    int dx[] = {  0,  1,  0, -1 };  // North, East, South, West around (y, x)
    for (int t = 0; t < TRIALS; t++) {
        int i, j;
        getRandomCell(i, j);
        if (board[i][j] == HIT) {
            int st = rand() % 4;
            for (int k = 0; k < 4; k++) {
                int r = (st + k) % 4;  // random start in dy, dx
                if (isValidCell(i + dy[r], j + dx[r]) && 
                    board[i + dy[r]][j + dx[r]] != HIT && 
                    board[i + dy[r]][j + dx[r]] != MISS) {
                    y = i + dy[r]; x = j + dx[r];
                    return true;
                }
            }
        }
    }
    // shoot at a random location
    getRandomCell(y, x);
    return true;
}

// Place ship on board in specified orientation (vertical or horizontal)
bool placeShip(char board[][N], int y, int x, char ship, bool vertical = false) {
    // If vertical = false, place ship horizontally.
    int size;
    int i;
    i = indexOf(ship);
    size = SHIP_SIZE[i];
    bool valid = isValidCell(y,x);
    
    if (!valid)
    { return false; }
    
    // First loop through all positions in given orientation to see if placing the ship is valid.
    for (int j = 0; j < size; j++) {
        if (vertical == true) {
            if (board[y+j][x] != BLANK || (y+j >= N))
            { return false; }
        } else if (vertical == false) {
            if (board[y][x+j] != BLANK || (x+j >= N))
            { return false; }
        }
    }

    // If all the positions are blank after validation from previous for-loop,
    // then we can update the board values.
    for (int k = 0; k < size; k++) {
        if (vertical) {
            board[y+k][x] = SHIP_TYPE[i][0];
        } else if (!vertical) {
            board[y][x+k] = SHIP_TYPE[i][0];
        }
    }
    return true;
}

// Randomly place all ships on board
void randomlyPlaceShips(char board[][N]) {
    for (int k = 0; k < 5; k++) {
        char ship = SHIP_TYPE[k][0];
        while (true) {
            int y, x;
            bool vertical = bool(rand() % 2);
            getRandomCell(y, x);
            if (placeShip(board, y, x, ship, vertical))
                break;
        }
    }
}

// Manually place all ships on board
void manuallyPlaceShips(char board[][N]) {
    for (int i = 0; i < 5; i++) {
        std::string ship = SHIP_TYPE[i];
        char shp = SHIP_TYPE[i][0], orientation;
        int x, y;
        bool vert;
        while (true) {
            std::cout << "Enter h/v and location for " << ship << ": ";
            std::cin >> orientation;
            getCellFromConsole(y, x);
            orientation = std::toupper(orientation);

            if (orientation == 'H') {
                vert = false;
                if (placeShip(board, y, x, shp, vert)) {
                    break;
                } else {
                    std::cout << "Invalid ship location!" << std::endl;
                }
            } else if (orientation == 'V') {
                vert = true;
                if (placeShip(board, y, x, shp, vert)) {
                    break;
                } else {
                    std::cout << "Invalid ship location!" << std::endl;
                }
            } else {
                std::cout << "Invalid ship location!" << std::endl;
            }
        }
    }
}