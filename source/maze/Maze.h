#ifndef Maze_h
#define Maze_h

#include "config_maze.h"

/**
 * @brief Status of wall
 */
typedef enum
{
	eWallError = mazeERROR, /* indicating error */
	empty = 0, /* indicating there is no wall */
	wall = 1, /* indicating there is wall */
	unknown = 2 /* indicating we haven't searched yet
	 so we don't know if there is a wall */
} wall_e;

/**
 * @brief Status of cell
 */
typedef enum
{
	eCellerror = mazeERROR, /* indicating error */
	unsearched = 0, /* there is unknown wall around a cell */
	searched = 1 /* all walls around a cell are searched */
} cell_e;

/**
 * @brief direction enumeration
 */
typedef enum Direction
{
	eDirError = -1, /* indicate error */
	row_plus = 0, /* direction in y-increasing way */
	col_plus = 1, /* direction in x-increasing way */
	row_minus = 2, /* direction in y-decreasing way */
	col_minus = 3 /* direction in x-decreasing way */
} direction_e;

/**
 * @brief position structure
 */
struct position_t
{
	int row; /* y (or row) index */
	int col; /* x (or column) index */

	position_t operator- (const position_t& rVal)
	{
		position_t result;
		result.col = col - rVal.col;
		result.row = row - rVal.row;
		return result;
	}

	bool operator== (const position_t& rVal)
	{
		return ((row == rVal.row) && (col == rVal.col));
	}
};

class Maze
{
private:
	wall_e rowWall[mazeMAX_ROW_SIZE + 1][mazeMAX_COL_SIZE]; /* walls in y-direction (or row-increasing) */
	wall_e colWall[mazeMAX_ROW_SIZE][mazeMAX_COL_SIZE + 1]; /* walls in x-direction (or column-increasing)*/
	cell_e cell[mazeMAX_ROW_SIZE][mazeMAX_COL_SIZE]; /* each cells in the maze */

	/**
	 * @brief      initialize the maze
	 */
	void initMaze ();

public:
	/**
	 * @brief      maze constructor
	 */
	Maze ();

	/**
	 * @brief      get status of a wall
	 *
	 * @param[in]  col   column index of cell
	 * @param[in]  row   row index of cell
	 * @param[in]  dir   direction of wall you are looking at
	 *
	 * @return     status of wall
	 */
	wall_e getWall (int row, int col, direction_e dir);

	/**
	 * @brief      get status of a cell
	 *
	 * @param[in]  col   column index of cell
	 * @param[in]  row   row index of cell
	 *
	 * @return     status of cell
	 */
	cell_e getCell (int row, int col);

	/**
	 * @brief      get status of a wall
	 *
	 * @param[in]  row     row index of cell
	 * @param[in]  col     column index of cell
	 * @param[in]  dir     direction of wall you are looking at
	 * @param[in]  status  staus of cell updating
	 *
	 * @return     mazeERROR (that is, -1) if failed
	 *             mazeSUCCESS otherwise
	 */
	int setWall (int row, int col, direction_e dir, wall_e status);

	/**
	 * @brief      update status of a cell
	 *
	 * @param[in]  row   row index of cell
	 * @param[in]  col   column index of cell
	 *
	 * @return     mazeERROR (that is, -1) if failed
	 *             mazeSUCCESS otherwise
	 */
	int updateCell (int row, int col);

	/**
	 * @brief      update status of all cell
	 */
	void updateCell ();
};
#endif
