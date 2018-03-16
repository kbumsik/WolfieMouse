/*
 * SimulMouse.hpp
 *
 *  Created on: Mar 5, 2017
 *      Author: vagrant
 */

#ifndef SIMULMOUSE_HPP_
#define SIMULMOUSE_HPP_

#include "FinderInterface.hpp"
#include "MoverInterface.hpp"
#include "Maze.hpp"
#include "IOInterface.hpp"

class SimulMouse: public FinderInterface, public MoverInterface
{
private:
	Maze maze;
public:
	// Constructor
	SimulMouse(char *mazeFile, IOInterface *fileIO, IOInterface *printIO);
    /* Overriding FinderInterface methods */
	Wall examineWall(int row, int col, Direction wallDir, PositionController &mousePos) override;
    /* Overriding MoverInterface methods */
	void moveTo(int row, int col, Direction destDir, PositionController &mousePos) override;
};

#endif /* SIMULMOUSE_HPP_ */
