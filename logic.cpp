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
    ifstream inFS;
    inFS.open(fileName);
    //Check if the file is valid
    if (!inFS.is_open()) {
        return nullptr;
    }
    //Check for errors
    inFS >> maxRow;
    if (inFS.fail()) {
        return nullptr;
    }
    inFS >> maxCol;
    if (inFS.fail()) {
        return nullptr;
    }
    inFS >> player.row;
    if (inFS.fail()) {
        return nullptr;
    }
    inFS >> player.col;
    if (inFS.fail()) {
        return nullptr;
    }
    if (maxRow < 1 || maxCol < 1) {
        return nullptr;
    }
    if (maxRow > INT32_MAX / double(maxCol) || maxCol > INT32_MAX / double(maxRow)) {
        return nullptr;
    }
    if (player.row < 0 || player.row >= maxRow || player.col < 0 || player.col >= maxCol) {
        return nullptr;
    }
    //Allocate the 2d array
    char** map = createMap(maxRow, maxCol);
    if (map == nullptr) {
        return nullptr;
    }
    char check;
    int countRow = 0;
    int countCol = 0;
    bool hasExit = false;
    bool hasDoor = false;
    for (int i = 0; i < maxRow; i++) {
        countRow++;
        for (int j = 0; j < maxCol; j++) {
            if(inFS >> check) {
                if (check == TILE_OPEN || check == TILE_PLAYER || check == TILE_TREASURE || check == TILE_AMULET || check == TILE_MONSTER || check == TILE_PILLAR || check == TILE_DOOR || check == TILE_EXIT) {
                    countCol++;
                    map[i][j] = check;
                    if (map[i][j] == TILE_EXIT) {
                        hasExit = true;
                    }
                    if (map[i][j] == TILE_DOOR) {
                        hasDoor = true;
                    }
                }
                else {
                    return nullptr;
                }
            }
            else if (!inFS.eof()) {
                return nullptr;
            }
        }
    }
    if (inFS >> check) {
        return nullptr;
    }
    if (countCol != maxCol * maxRow) {
        return nullptr;
    }
    if (countRow > INT32_MAX / countCol) {
        return nullptr;
    }
    if (!hasExit && !hasDoor) {
        return nullptr;
    }
    map[player.row][player.col] = TILE_PLAYER;
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
    switch(input) {
        case MOVE_UP:
            nextRow--;
            break;
        case MOVE_DOWN:
            nextRow++;
            break;
        case MOVE_RIGHT:
            nextCol++;
            break;
        case MOVE_LEFT:
            nextCol--;
            break;
    }
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
    if (maxRow <= 0 || maxCol <= 0) {
        return nullptr;
    }
    if (maxRow > INT32_MAX / double(maxCol) || maxCol > INT32_MAX / double(maxRow)) {
        return nullptr;
    }
    char** Map = new char*[maxRow];
    for (int i = 0; i < maxRow; i++) {
        Map[i] = new char[maxCol];
        for (int j = 0; j < maxCol; j++) {
            Map[i][j] = TILE_OPEN;
        }
    }
    return Map;
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
    if (map != nullptr && maxRow > 0) {
        for (int i = 0; i < maxRow; i++) {
            delete [] map[i];
        }
        delete [] map;
        map = nullptr;
        maxRow = 0;
    }
    map = nullptr;
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
    if (map == nullptr) {
        return nullptr;
    }
    if (maxRow <= 0 || maxCol <= 0) {
        return nullptr;
    }
    int newRow = maxRow * 2;
    int newCol = maxCol * 2;
    char** resize = createMap(newRow, newCol);
    if (resize == nullptr) {
        return nullptr;
    }
    int row = 0;
    int col = 0; 
    bool player = false;
    //Copy into subarrays
    for (int a = 0; a < 4; a++) {    
        for (int i = 0; i < maxRow; i++) {
            for (int j = 0; j < maxCol; j++) {
                if (a < 2) {
                    row = 0 + i;
                }
                else {
                    row = maxRow + i;
                }
                col = ((a % 2) * maxCol) + j;               
                if(map[i][j] == TILE_PLAYER && player){
                    map[i][j] = TILE_OPEN;
                }
                resize[row][col] =  map[i][j] ;
                if (map[i][j] == TILE_PLAYER) {
                    player = true;
                }
            }
        }
    }
    deleteMap(map,maxRow);
    maxRow = newRow;
    maxCol = newCol;

    return resize;
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
    //Check if outside of bound
    if (nextRow >= maxRow || nextRow < 0) {
        return STATUS_STAY;
    }
    if (nextCol >= maxCol || nextCol < 0) {
        return STATUS_STAY;
    }
    //Check if the player is on the impassible tile
    if (map[nextRow][nextCol] == TILE_MONSTER || map[nextRow][nextCol] == TILE_PILLAR) {
        return STATUS_STAY;
    }
    //Check status of the player
    int status = STATUS_MOVE;
    if (map[nextRow][nextCol] == TILE_TREASURE) {
        player.treasure++;
        status = STATUS_TREASURE;
    }
    else if (map[nextRow][nextCol] == TILE_AMULET) {
        status = STATUS_AMULET;
    }
    else if (map[nextRow][nextCol] == TILE_DOOR) {
        status = STATUS_LEAVE;
    }
    else if (map[nextRow][nextCol] == TILE_EXIT) {
        if (player.treasure > 0) {
            status = STATUS_ESCAPE;
        }
        else {
            return STATUS_STAY;
        }
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
    //Check for up monster
    for (int i = player.row-1; i >= 0; i--) {
        if (map[i][player.col] == TILE_PILLAR) {
            break;
        }
        else if (map[i][player.col] == TILE_MONSTER) {
            map[i][player.col] = TILE_OPEN;
            map[i+1][player.col] = TILE_MONSTER;
        }
    }  
    //Check for down monster
    for (int i = player.row+1; i < maxRow; i++) {
        if (map[i][player.col] == TILE_PILLAR) {
            break;
        }
        else if (map[i][player.col] == TILE_MONSTER) {
            map[i][player.col] = TILE_OPEN;
            map[i-1][player.col] = TILE_MONSTER;
        }
    }  
    //Check for right monster
    for (int i = player.col+1; i < maxCol; i++) {
        if (map[player.row][i] == TILE_PILLAR) {
            break;
        }
        else if (map[player.row][i] == TILE_MONSTER) {
            map[player.row][i] = TILE_OPEN;
            map[player.row][i-1] = TILE_MONSTER;
        }
    } 
    //Check for left monster 
    for (int i = player.col-1; i >= 0; i--) {
        if (map[player.row][i] == TILE_PILLAR) {
            break;
        }
        else if (map[player.row][i] == TILE_MONSTER) {
            map[player.row][i] = TILE_OPEN;
            map[player.row][i+1] = TILE_MONSTER;
        }
    } 
    return map[player.row][player.col] == TILE_MONSTER;
}
