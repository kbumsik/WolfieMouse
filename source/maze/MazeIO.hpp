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
#include "PositionController.hpp"
#include "IOInterface.hpp"

#include <vector>
#include <stdio.h>
#include <stddef.h>

// Forward declaration
class Maze;

/*******************************************************************************
 * Class Declaration
 ******************************************************************************/
class MazeIO
{
private:
    Maze *maze;
    int maxRowSize;
    int maxColSize;
    PositionController mousePosition;
    std::vector<Position> destinations;
    /* Printing buffer (the last + 1 is for newline character */
    char buffer[(CONFIG_MAX_ROW_SIZE * 2 + 1) * (CONFIG_MAX_COL_SIZE * 3 + 1 + 1)];
    // IOInterface objects for IO
    IOInterface *fileIO;
    IOInterface *printIO;
    // printing helper
    void writeBufferFromMaze (bool isShowMouse);
    void writeIOFromBuffer (IOInterface *io);
    void printCell(int row, int col, bool isShowMouse, char* buf);
public:
    // Constructor
    MazeIO(Maze *mazePtr, IOInterface *fileIO, IOInterface *printIO);
    // Mouse Position
    Position getMousePosition(void);
    void setMousePosition(Position pos);
    void setMousePosition(PositionController posCon);
    // Mouse Destinations
    void setDestinations(const std::vector<Position> &des);
    bool positionIsDestination(Position pos);
    // Actual IO
    void printMaze(void);
    void loadMaze(char* fileName);
    void saveMaze(char* fileName);
};

/*******************************************************************************
 * Inline function definition
 ******************************************************************************/
 

#endif /* MAZEIO_HPP_ */
