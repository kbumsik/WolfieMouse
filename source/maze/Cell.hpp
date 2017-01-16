/*
 * Cell.h
 *
 *  Created on: Jan 16, 2017
 *      Author: vagrant
 */

#ifndef CELL_HPP_
#define CELL_HPP_

/**
 * @brief Status of cell
 */
struct Cell
{
    int distance;
    enum common_maze_status status;
    bool isMouse, isGoal, isStart;
    /** FIXME: maybe put isDestination? */
    /** TODO: put turning? */
};

#endif /* CELL_HPP_ */
