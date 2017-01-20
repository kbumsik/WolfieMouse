#ifndef Maze_h
#define Maze_h

#include <Cell.hpp>
#include "common_maze.h"
#include "config_maze.h"
#include <stdio.h>
#include "Direction.hpp"
#include "Wall.hpp"
#include "MazeIO.hpp"


#define MAZE_IS_ROW_SAFE_FORWARD(row)	(row < (CONFIG_MAX_ROW_SIZE - 1))
#define MAZE_IS_ROW_SAFE_BACKWARD(row)	(row > 0)
#define MAZE_IS_ROW_SAFE_TO_MOVE(row)	(MAZE_IS_ROW_SAFE_FORWARD(row) && MAZE_IS_ROW_SAFE_BACKWARD(row))

#define MAZE_IS_COL_SAFE_FORWARD(col)	(col < (CONFIG_MAX_COL_SIZE - 1))
#define MAZE_IS_COL_SAFE_BACKWARD(col)	(col > 0)
#define MAZE_IS_COL_SAFE_TO_MOVE(col)	(MAZE_IS_COL_SAFE_FORWARD(col) && MAZE_IS_COL_SAFE_BACKWARD(col))

#define MAZE_IS_POS_SAFE_TO_MOVE(row, col)	(MAZE_IS_ROW_SAFE_TO_MOVE(row) && MAZE_IS_COL_SAFE_TO_MOVE(col))

#define MAZE_IS_ROW_OUT_BOUNDS(row)	((row >= CONFIG_MAX_ROW_SIZE) || (row < 0))
#define MAZE_IS_COL_OUT_BOUNDS(col)	((col >= CONFIG_MAX_COL_SIZE) || (col < 0))
#define MAZE_IS_POS_OUT_BOUNDS(row, col)	(MAZE_IS_ROW_OUT_BOUNDS(row) || MAZE_IS_COL_OUT_BOUNDS(col))


/*******************************************************************************
 * Class Declaration
 ******************************************************************************/
class Maze
{
    friend class MazeIO;
private:
    Wall rowWall[CONFIG_MAX_ROW_SIZE + 1][CONFIG_MAX_COL_SIZE]; /* walls in y-direction (or row-increasing) */
    Wall colWall[CONFIG_MAX_ROW_SIZE][CONFIG_MAX_COL_SIZE + 1]; /* walls in x-direction (or column-increasing)*/
    Cell cell[CONFIG_MAX_ROW_SIZE][CONFIG_MAX_COL_SIZE]; /* each cells in the maze */
    // Maze printer
    MazeIO mazeIO;
public:
    int index_goal_row;
    int index_goal_col;
    int index_start_row;
    int index_start_col;

    /* Constructors */
    Maze();
    Maze(char* filename);
    /* Position of mouse */
    void setMouse(int row, int col);
    void resetMouse(int row, int col);
    /* Related to wall */
    Wall getWall(int row, int col, Direction dir);
    int setWall(int row, int col, Direction dir, Wall status);
    /* Related to Cell */
    Cell getCell(int row, int col);
    int updateCell(int row, int col);
    void updateCell();
    /* getter and setter of Distance of cell */
    inline int getDistance(int row, int col);
    inline int setDistance(int row, int col, int dis);
    /* IO Related */
    void readMazeFromFile(char* fileName);
    void printMaze();
    void saveMazeFile(char* fileName);
};

/*******************************************************************************
 * Inline function definition
 ******************************************************************************/
inline int Maze::getDistance(int row, int col)
{
    if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
        printf("invalid cell!\n");
        return COMMON_MAZE_ERROR;
    }
    return cell[row][col].distance;
}

inline int Maze::setDistance(int row, int col, int dis)
{
    if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
        printf("invalid cell!\n");
        return COMMON_MAZE_ERROR;
    }
    cell[row][col].distance = dis;
    return COMMON_MAZE_SUCCESS;
}

#endif
