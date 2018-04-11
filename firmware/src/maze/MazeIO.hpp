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
    char buffer[CONFIG_MAZE_IO_BUFFER_SIZE];
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
    void printMazeWithPositions(std::vector<Position> &positions);
    void loadMazeFromString(char* str);
    void loadMazeFromFile(char* fileName);
    void saveMazeToFile(char* fileName);
};

/*******************************************************************************
 * Inline function definition
 ******************************************************************************/
 

#endif /* MAZEIO_HPP_ */
