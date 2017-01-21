/*
 * Cell.h
 *
 *  Created on: Jan 16, 2017
 *      Author: vagrant
 */

#ifndef CELL_HPP_
#define CELL_HPP_

#include "common_maze.h"


#define CELL_DISTANCE_START     0
#define CELL_DISTANCE_UNREACHED -1
#define CELL_DISTANCE_ERROR     -2

enum Status
{
    cellError = COMMON_MAZE_ERROR, /* indicating error */
    unsearched = 0, /* there is unknown wall around a cell */
    searched = 1 /* all walls around a cell are searched */
};

enum Attribute
{
    nothing, goal, start
};

/**
 * @brief Status of cell
 */
struct Cell
{
    int distance;
    Status status;
    Attribute attribute;
    bool isMouse; //, isGoal, isStart;
    /** FIXME: maybe put isDestination? */
    /** TODO: put turning? */
};

#endif /* CELL_HPP_ */
