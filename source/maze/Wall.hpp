/*
 * wall_enum.h
 *
 *  Created on: Jan 16, 2017
 *      Author: vagrant
 */

#ifndef WALL_HPP_
#define WALL_HPP_

#include "common_maze.h"

/**
 * @brief Status of wall
 */
enum Wall
{
    wallError = COMMON_MAZE_ERROR, /* indicating error */
    empty = 0, /* indicating there is no wall */
    wall = 1, /* indicating there is wall */
    unknown = 2 /* indicating we haven't searched yet
     so we don't know if there is a wall */
};

#endif /* WALL_HPP_ */
