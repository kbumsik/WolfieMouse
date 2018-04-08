/*
 * SimulMouse.cc
 *
 *  Created on: Mar 5, 2017
 *      Author: vagrant
 */

#include "SimulMouse.hpp"

/*******************************************************************************
 * Constructor
 ******************************************************************************/
SimulMouse::SimulMouse(char *mazeFile, IOInterface *fileIO, IOInterface *printIO):
	maze(mazeFile, fileIO, printIO)
{
    // The initializer does it all
}

SimulMouse::SimulMouse(Maze::StringMaze *stringMaze, IOInterface *fileIO, IOInterface *printIO):
	maze(stringMaze, fileIO, printIO)
{
    // The initializer does it all
}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/
/* Overriding FinderInterface methods */
Wall SimulMouse::examineWall(int row, int col, Direction wallDir, PositionController &mousePos)
{
	return maze.getWall(row, col, wallDir);
}

/* Overriding MoverInterface methods */
void SimulMouse::moveTo(int row, int col, Direction destDir, PositionController &mousePos)
{
	return;
}

void SimulMouse::rotateTo(Direction destDir, PositionController &mousePos)
{
	return;
}