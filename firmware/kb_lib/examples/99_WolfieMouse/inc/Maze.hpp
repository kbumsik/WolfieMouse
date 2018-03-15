#ifndef Maze_h
#define Maze_h

#include <config_maze.hpp>
#include <stdio.h>

/**
 * @brief Status of wall
 */
enum wall
{
	eWallError = mazeERROR, /* indicating error */
	empty = 0, /* indicating there is no wall */
	wall = 1, /* indicating there is wall */
	unknown = 2 /* indicating we haven't searched yet
	 so we don't know if there is a wall */
};


enum status
{
	eCellerror = mazeERROR, /* indicating error */
	unsearched = 0, /* there is unknown wall around a cell */
	searched = 1 /* all walls around a cell are searched */
};

/**
 * @brief Status of cell
 */
struct cell{
	int distance;
	enum status status;
	bool isMouse, isGoal, isStart;
	/** FIXME: maybe put isDestination? */
	/** TODO: put turning? */
};

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
} dir_e;

/**
 * @brief position structure
 */
struct pos_t
{
	int row; /* y (or row) index */
	int col; /* x (or column) index */

	pos_t operator- (const pos_t& rVal)
	{
		pos_t result;
		result.col = col - rVal.col;
		result.row = row - rVal.row;
		return result;
	}

	bool operator== (const pos_t& rVal)
	{
		return ((row == rVal.row) && (col == rVal.col));
	}
};

class Maze
{
private:
	enum wall rowWall[mazeMAX_ROW_SIZE + 1][mazeMAX_COL_SIZE]; /* walls in y-direction (or row-increasing) */
	enum wall colWall[mazeMAX_ROW_SIZE][mazeMAX_COL_SIZE + 1]; /* walls in x-direction (or column-increasing)*/
	struct cell cell[mazeMAX_ROW_SIZE][mazeMAX_COL_SIZE]; /* each cells in the maze */

	void init();

	void writeMazeToFile(void *pFile, bool isShowMouse);

	void printCell(int row, int col, bool isShowMouse, void *pFile);
public:
	int index_goal_row;
	int index_goal_col;
	int index_start_row;
	int index_start_col;

	/**
	 * @brief      maze constructor
	 */
	Maze ();

	/**
	 * @brief      maze constructor. Maze is built based on the file
	 *
	 * @param      filename  the name of the file to read
	 */
	Maze (char* filename);

	/**
	 * @brief      get status of a wall
	 *
	 * @param[in]  col   column index of cell
	 * @param[in]  row   row index of cell
	 * @param[in]  dir   direction of wall you are looking at
	 *
	 * @return     status of wall
	 */
	enum wall getWall (int row, int col, dir_e dir);

	/**
	 * @brief      get status of a cell
	 *
	 * @param[in]  col   column index of cell
	 * @param[in]  row   row index of cell
	 *
	 * @return     status of cell
	 */
	struct cell getCell (int row, int col);

	void setMouse(int row, int col);
	void resetMouse(int row, int col);
	/**
	 * @brief      set status of a wall
	 *
	 * @param[in]  row     row index of cell
	 * @param[in]  col     column index of cell
	 * @param[in]  dir     direction of wall you are looking at
	 * @param[in]  status  staus of cell updating
	 *
	 * @return     mazeERROR (that is, -1) if failed
	 *             mazeSUCCESS otherwise
	 */
	int setWall (int row, int col, dir_e dir, enum wall status);

	inline int setDistance(int row, int col, int dis)
	{
		if (mazeIS_POS_OUT_BOUNDS(row, col))
		{
			printf("invalid cell!\n");
			return mazeERROR;
		}
		cell[row][col].distance = dis;
		return mazeSUCCESS;
	}

	inline int getDistance(int row, int col)
	{
		if (mazeIS_POS_OUT_BOUNDS(row, col))
		{
			printf("invalid cell!\n");
			return mazeERROR;
		}
		return cell[row][col].distance;
	}

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
	/**
	 * @brief      construct Maze from file
	 *
	 * @param      fileName  filename to construct
	 */
	void readMazeFromFile(char* fileName);

	/**
	 * @brief      print the current maze
	 */
	void printMaze();

	/**
	 * @brief      save the maze as file
	 *
	 * @param      fileName  filename to save
	 */
	void saveMazeFile(char* fileName);
};
#endif
