/*
 * maze_common.h
 *
 *  Created on: Jan 15, 2017
 *      Author: vagrant
 */

#ifndef COMMON_MAZE_H_
#define COMMON_MAZE_H_

#define COMMON_MAZE_SUCCESS  1
#define COMMON_MAZE_ERROR	-2

enum common_maze_status
{
    cellError = COMMON_MAZE_ERROR, /* indicating error */
    unsearched = 0, /* there is unknown wall around a cell */
    searched = 1 /* all walls around a cell are searched */
};

#endif /* COMMON_MAZE_H_ */
