#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {

    string line; // Buffer for current line
    ifstream file(fileName); // Read file into input file stream

    if (!file.is_open())
    {
        cout << "Error: Could not open file " << fileName << endl;
        return nullptr;
    }

    // Check if the file is empty
    if (file.peek() == std::ifstream::traits_type::eof()) 
    {
        cout << "Error: File is empty" << endl;
        return nullptr;
    }

    if (!(file >> maxRow >> maxCol))
    {
        cout << "Error: Invalid maxRow or maxCol values in the file" << endl;
        return nullptr;
    }

    // Check if maxRow and maxCol are within reasonable limits
    if (maxRow <= 0 || maxCol <= 0 || static_cast<int64_t>(maxRow) * static_cast<int64_t>(maxCol) > INT32_MAX) 
    {
        cout << "Error: Invalid dimensions for the map" << endl;
        return nullptr;
    }

    if (!(file >> player.row >> player.col))
    {
        cout << "Error: Invalid player row or player column values in the file" << endl;
        return nullptr;
    }

    // Check if player.row and player.col are within the bounds of map
    if (player.row < 0 || player.col < 0 || player.row >= maxRow || player.col >= maxCol) 
    {
        cout << "Error: Invalid player position" << endl;
        return nullptr;
    }

    // Initialize a pointer to array of char*
    // Allocate memory for "maxRow" row pointers
    char** map = createMap(maxRow, maxCol);
    if (map == nullptr)
    {
        cout << "Error: Could not allocate memory for the map" << endl;
        return nullptr;
    }

    const char validTiles[] = {TILE_OPEN, TILE_MONSTER, TILE_PILLAR, TILE_DOOR, TILE_EXIT, TILE_TREASURE, TILE_AMULET}; 

    // Lambda function to check if a tile is valid
    auto isValidTile = [&](char tile) 
    {
        for (int i = 0; i < 8; ++i) {
            if (tile == validTiles[i]) return true;
        }
        return false;
    };

    // Iterate through every row
    int tileCount = 0;
    for (int row = 0; row < maxRow; ++row)
    {
        for (int col = 0; col < maxCol; ++col)
        {
            if (!(file >> map[row][col]))
            {
                cout << "Error: Map data is incomplete or invalid" << std::endl;
                // Free up allocated memory for map before return 
                deleteMap(map, maxRow);
                return nullptr;
            }

            if (!isValidTile(map[row][col])) {
                cout << "Error: Invalid character in the map" << endl;
                deleteMap(map, maxRow);
                return nullptr;
            }
            tileCount++;
        }
    }

    // Check if the number of tiles matches maxRow * maxCol
    if (tileCount != maxRow * maxCol) {
        cout << "Error: Number of tiles in the file does not match map dimensions" << endl;
        deleteMap(map, maxRow);
        return nullptr;
    }

    // Make sure that the player is placed on a valid tile
    if (map[player.row][player.col] != TILE_OPEN) 
    {
        cout << "Error: Player starting position is invalid" << endl;
        deleteMap(map, maxRow);
        return nullptr;
    }

    // Ensure no extra data in the file
    char extra;
    if (file >> extra) 
    {
        cout << "Error: File contains extra data beyond the map" << endl;
        deleteMap(map, maxRow);
        return nullptr;
    }

    map[player.row][player.col] = TILE_PLAYER; // set player position

    return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    
    if (input == MOVE_UP) nextRow--;
    else if (input == MOVE_DOWN) nextRow++;
    else if (input == MOVE_LEFT) nextCol--;
    else if (input == MOVE_RIGHT) nextCol++;
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    char** map = new char*[maxRow];
    for (int row = 0; row < maxRow; ++row)
    {
        map[row] = new char[maxCol];
        for (int col = 0; col < maxCol; ++col)
        {
            map[row][col] = TILE_OPEN;
        }
    }

    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if (map != nullptr)
    {
        for (int row = 0; row < maxRow; ++row)
        {
            delete [] map[row];
        }
        delete [] map;
        map = nullptr;
    }
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    if (map == nullptr || maxRow <= 0 || maxCol <= 0) return nullptr;

    int oldRow = maxRow;
    int oldCol = maxCol;
    maxRow *= 2;
    maxCol *= 2;

    char** newMap = new char*[maxRow];
    for (int row = 0; row < maxRow; ++row) newMap[row] = new char[maxCol];

    for (int row = 0; row < maxRow; ++row) 
    {
        for (int col = 0; col < maxCol; ++col) 
        {
            if (row < oldRow && col < oldCol) 
            {
                // Subarray A (top-left): copy exactly from the original map
                newMap[row][col] = map[row][col];
            }
            else if (row < oldRow && col >= oldCol) 
            {
                // Subarray B (top-right)
                newMap[row][col] = (map[row][col - oldCol] == TILE_PLAYER) ? TILE_OPEN : map[row][col - oldCol];
            }
            else if (row >= oldRow && col < oldCol) 
            {
                // Subarray C (bottom-left)
                newMap[row][col] = (map[row - oldRow][col] == TILE_PLAYER) ? TILE_OPEN : map[row - oldRow][col];
            }
            else 
            {
                // Subarray D (bottom-right)
                newMap[row][col] = (map[row - oldRow][col - oldCol] == TILE_PLAYER) ? TILE_OPEN : map[row - oldRow][col - oldCol];
            }
        }
    }

    // Deallocate the old map
    deleteMap(map, oldRow);

    return newMap;  // Return the new enlarged map
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    if (nextRow < 0 || nextCol < 0 || nextRow >= maxRow || nextCol >= maxCol) return STATUS_STAY;

    int status = STATUS_MOVE;
    char tile = map[nextRow][nextCol];

    if (tile == TILE_PILLAR || tile == TILE_MONSTER) return STATUS_STAY;
    
    if (tile == TILE_TREASURE)
    {
        player.treasure++;
        status = STATUS_TREASURE;
    }
    if (tile == TILE_AMULET) status = STATUS_AMULET;
    if (tile == TILE_DOOR) status = STATUS_LEAVE;
    if (tile == TILE_EXIT) 
    {
        if (player.treasure > 0) status = STATUS_ESCAPE;
        else return STATUS_STAY;
    }

    map[player.row][player.col] = TILE_OPEN;
    map[nextRow][nextCol] = TILE_PLAYER;
    player.row = nextRow;
    player.col = nextCol;

    return status;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    // Check below player
    for (int row = 1; player.row + row < maxRow; ++row)
    {
        char tile = map[player.row + row][player.col];
        if (tile == TILE_PILLAR) break;
        if (tile == TILE_MONSTER)
        {
            map[player.row + row][player.col] = TILE_OPEN;
            map[(player.row + row) - 1][player.col] = TILE_MONSTER; // Move monster up
        }
    }
    // Check above player
    for (int row = 1; player.row - row >= 0; ++row)
    {
        char tile = map[player.row - row][player.col];
        if (tile == TILE_PILLAR) break;
        if (tile == TILE_MONSTER)
        {
            map[player.row - row][player.col] = TILE_OPEN;
            map[(player.row - row) + 1][player.col] = TILE_MONSTER; // Move monster down
        }
    }
    // Check right of player
    for (int col = 1; col + player.col < maxCol; ++col)
    {
        char tile = map[player.row][player.col + col];
        if (tile == TILE_PILLAR) break;
        if (tile == TILE_MONSTER)
        {
            map[player.row][player.col + col] = TILE_OPEN;
            map[player.row][(player.col + col) - 1] = TILE_MONSTER; // Move monster left
        }
    }
    // Check left of player
    for (int col = 1; player.col - col >= 0; ++col)
    {
        char tile = map[player.row][player.col - col];
        if (tile == TILE_PILLAR) break;
        if (tile == TILE_MONSTER)
        {
            map[player.row][player.col - col] = TILE_OPEN;
            map[player.row][(player.col - col) + 1] = TILE_MONSTER; // Move monster right
        }
    }
    return map[player.row][player.col] == TILE_MONSTER;
}
