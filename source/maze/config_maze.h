/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_MAZE_H
#define __CONFIG_MAZE_H

#define mazeMAX_ROW_SIZE	16
#define mazeMAX_COL_SIZE	16
#define mazeQUEUE_MAX_BUFFER (mazeMAX_COL_SIZE*mazeMAX_ROW_SIZE)
#define mazeDIRECTION_START row_plus

#define mazeIS_ROW_SAFE_TO_MOVE_FORWARD(row)	(row < (mazeMAX_ROW_SIZE - 1))
#define mazeIS_ROW_SAFE_TO_MOVE_BACKWARD(row)	(row > 0)
#define mazeIS_ROW_SAFE_TO_MOVE(row)	(mazeIS_ROW_SAFE_TO_MOVE_FORWARD(row) && mazeIS_ROW_SAFE_TO_MOVE_BACKWARD(row))		

#define mazeIS_COL_SAFE_TO_MOVE_FORWARD(col)	(col < (mazeMAX_COL_SIZE - 1))
#define mazeIS_COL_SAFE_TO_MOVE_BACKWARD(col)	(col > 0)
#define mazeIS_COL_SAFE_TO_MOVE(col)	(mazeIS_COL_SAFE_TO_MOVE_FORWARD(col) && mazeIS_COL_SAFE_TO_MOVE_BACKWARD(col))

#define mazeIS_POS_SAFE_TO_MOVE(row, col)	(mazeIS_ROW_SAFE_TO_MOVE(row) && mazeIS_COL_SAFE_TO_MOVE(col))

#define mazeIS_ROW_OUT_BOUNDS(row)	((row >= mazeMAX_ROW_SIZE) || (row < 0))
#define mazeIS_COL_OUT_BOUNDS(col)	((col >= mazeMAX_COL_SIZE) || (col < 0))
#define mazeIS_POS_OUT_BOUNDS(row, col)	(mazeIS_ROW_OUT_BOUNDS(row) || mazeIS_COL_OUT_BOUNDS(col))

#define mazeSTART_DISTANCE 0
#define UNREACHED	-1

#define mazeSUCCESS	1
#define mazeERROR	-2

#endif /* __CONFIG_MAZE_H */
