#include "common_maze.h"
#include "Maze.hpp"

/*******************************************************************************
 * Constructor
 ******************************************************************************/
/**
 * @brief      maze constructor
 */
Maze::Maze(IOInterface *fileIO, IOInterface *printIO) :
    Maze(NULL, fileIO, printIO)
{
}

/**
 * @brief      maze constructor. Maze is built based on the file
 *
 * @param      filename  the name of the file to read
 */
Maze::Maze(char *filename, IOInterface *fileIO, IOInterface *printIO) :
    mazeIO(this, fileIO, printIO),
	startPos(CONFIG_DEFAULT_MAZE_START)
{
    int i = 0;
    int j = 0;
    int k = 0;

    /* init the wall with unknown */
    for (i = 0; i < CONFIG_MAX_ROW_SIZE; i++) {
        for (j = 0; j < CONFIG_MAX_COL_SIZE; j++) {
            setDistance(i, j, CELL_DISTANCE_UNREACHED);
            for (k = (int) row_plus; k <= (int) col_minus; k++) {
                setWall(i, j, (Direction) k, unknown);
            }
        }
    }

    /* wrap the wall */
    for (i = 0; i < CONFIG_MAX_ROW_SIZE; i++) {
        setWall(i, 0, col_minus, wall);
        setWall(i, CONFIG_MAX_ROW_SIZE - 1, col_plus, wall);
    }
    for (j = 0; j < CONFIG_MAX_COL_SIZE; j++) {
        setWall(0, j, row_minus, wall);
        setWall(CONFIG_MAX_COL_SIZE - 1, j, row_plus, wall);
    }

    /* fill the first cell */
    CONFIG_SET_DEFAULT_FIRST_CELL();

    /* fill walls around the goal */
    CONFIG_SET_DEFAULT_GOAL_CELLS();

    /* update the cell */
    updateCell();

    /* Load maze */
    //readMazeFromFile(filename);
    /* Init default goals if no file  */
    if (filename == NULL) {
    	goalPos = CONFIG_DEFAULT_MAZE_GOAL;
    }

}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/

/**
 * @brief      get status of a wall
 *
 * @param[in]  col   column index of cell
 * @param[in]  row   row index of cell
 * @param[in]  dir   direction of wall you are looking at
 *
 * @return     status of wall
 */
Wall Maze::getWall(int row, int col, Direction dir)
{
    if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
        return (Wall) COMMON_MAZE_ERROR;
    }
    switch (dir) {
    case row_plus:
        return rowWall[row + 1][col];
    case col_plus:
        return colWall[row][col + 1];
    case row_minus:
        return rowWall[row][col];
    case col_minus:
        return colWall[row][col];
    default:
        return (Wall) COMMON_MAZE_ERROR;
    }
    return (Wall) COMMON_MAZE_ERROR;
}

/**
 * @brief      get status of a cell
 *
 * @param[in]  col   column index of cell
 * @param[in]  row   row index of cell
 *
 * @return     status of cell
 */
Cell Maze::getCell(int row, int col)
{
    if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
        return Cell { CELL_DISTANCE_ERROR, cellError, nothing };
    } else {
        return cell[row][col];
    }
}

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
int Maze::setWall(int row, int col, Direction dir, Wall status)
{
    if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
        return COMMON_MAZE_ERROR;
    }
    switch (dir) {
    case row_plus:
        rowWall[row + 1][col] = status;
        return COMMON_MAZE_SUCCESS;
    case col_plus:
        colWall[row][col + 1] = status;
        return COMMON_MAZE_SUCCESS;
    case row_minus:
        rowWall[row][col] = status;
        return COMMON_MAZE_SUCCESS;
    case col_minus:
        colWall[row][col] = status;
        return COMMON_MAZE_SUCCESS;
    default:
        return COMMON_MAZE_ERROR;
    }
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
int Maze::updateCell(int row, int col)
{
    if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
        return COMMON_MAZE_ERROR;
    }
    /* checking status */
    if (   Maze::getWall(row, col, row_plus) != unknown
		&& Maze::getWall(row, col, col_plus) != unknown
		&& Maze::getWall(row, col, row_minus)!= unknown
		&& Maze::getWall(row, col, col_minus)!= unknown) {
        cell[row][col].status = searched;
    } else {
        cell[row][col].status = unsearched;
    }

    if ((row == startPos.row) && (col == startPos.col)) {
        /* checking start */
        cell[row][col].attribute = start;
        return COMMON_MAZE_SUCCESS;
    }

    for (int i = 0; i < goalPos.size(); i++) {
    	  /* checking goal */
    	if ((row == goalPos[i].row) && (col == goalPos[i].col)) {
    		cell[row][col].attribute = goal;
    		return COMMON_MAZE_SUCCESS;
    	}
    }

    cell[row][col].attribute = nothing;
    /* TODO: checking mouse */
    return COMMON_MAZE_SUCCESS;
}

/**
 * @brief      update status of all cell
 */
void Maze::updateCell()
{
    int i = 0;
    int j = 0;
    for (i = 0; i < CONFIG_MAX_ROW_SIZE; i++) {
        for (j = 0; j < CONFIG_MAX_COL_SIZE; j++) {
            Maze::updateCell(i, j);
        }
    }
}

/**
 * @brief      construct Maze from file
 *
 * @param      fileName  filename to construct
 */
void Maze::readMazeFromFile(char* fileName)
{
    mazeIO.loadMaze(fileName);
}

/**
 * @brief      print the current maze
 */
void Maze::printMaze()
{
    mazeIO.printMaze();
}

/**
 * @brief      save the maze as file
 *
 * @param      fileName  filename to save
 */
void Maze::saveMazeFile(char* fileName)
{
    mazeIO.saveMaze(fileName);
}

/*******************************************************************************
 * Private Methods
 ******************************************************************************/
