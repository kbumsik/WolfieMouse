/*
 * mazeIO.hpp
 *
 *  Created on: Jan 17, 2017
 *      Author: vagrant
 */

#ifndef MAZEIO_HPP_
#define MAZEIO_HPP_

#include "config_maze.h"
#include "Wall.hpp"
#include "Cell.hpp"
#include "Direction.hpp"
#include "Position.hpp"
#include <stdio.h>
#include <stddef.h>

// Forward declaration
class Maze;

/*******************************************************************************
 * Class Declaration
 ******************************************************************************/
class MazeIO
{
    friend class Maze;
private:
    Maze *maze;
    int maxRowSize;
    int maxColSize;
    Position mousePosition;
    /* Printing buffer (the last + 1 is for newline charater */
    char buffer[(CONFIG_MAX_ROW_SIZE * 2 + 1) * (CONFIG_MAX_COL_SIZE * 2 + 1 + 1)];
    // printing helper
    void writeBufferFromMaze (bool isShowMouse);
    void writeFileFromBuffer (FILE *pFile);
    void printCell(int row, int col, bool isShowMouse, char* buf);
public:
    // Constructor
    MazeIO(Maze *mazePtr);
    // mouse
    Position getMousePosition(void);
    void setMousePosition(Position pos);
    // Actual IO
    void printMaze(void);
    void loadMaze(char* fileName);
    void saveMaze(char* fileName);
};

/*******************************************************************************
 * Inline function definition
 ******************************************************************************/
 

#endif /* MAZEIO_HPP_ */
