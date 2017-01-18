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


class Maze
{
    friend class MazeIO;
private:
    Wall rowWall[CONFIG_MAX_ROW_SIZE + 1][CONFIG_MAX_COL_SIZE]; /* walls in y-direction (or row-increasing) */
    Wall colWall[CONFIG_MAX_ROW_SIZE][CONFIG_MAX_COL_SIZE + 1]; /* walls in x-direction (or column-increasing)*/
    Cell cell[CONFIG_MAX_ROW_SIZE][CONFIG_MAX_COL_SIZE]; /* each cells in the maze */
    // Maze printer
    MazeIO mazeIO;
    // initialization
    void init();
public:
    int index_goal_row;
    int index_goal_col;
    int index_start_row;
    int index_start_col;

    /**
     * @brief      maze constructor
     */
    Maze();

    /**
     * @brief      maze constructor. Maze is built based on the file
     *
     * @param      filename  the name of the file to read
     */
    Maze(char* filename);

    /**
     * @brief      get status of a wall
     *
     * @param[in]  col   column index of cell
     * @param[in]  row   row index of cell
     * @param[in]  dir   direction of wall you are looking at
     *
     * @return     status of wall
     */
    Wall getWall(int row, int col, Direction dir);

    /**
     * @brief      get status of a cell
     *
     * @param[in]  col   column index of cell
     * @param[in]  row   row index of cell
     *
     * @return     status of cell
     */
    Cell getCell(int row, int col);

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
    int setWall(int row, int col, Direction dir, Wall status);

    inline int setDistance(int row, int col, int dis)
    {
        if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
            printf("invalid cell!\n");
            return COMMON_MAZE_ERROR;
        }
        cell[row][col].distance = dis;
        return COMMON_MAZE_SUCCESS;
    }

    inline int getDistance(int row, int col)
    {
        if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
            printf("invalid cell!\n");
            return COMMON_MAZE_ERROR;
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
    int updateCell(int row, int col);

    /**
     * @brief      update status of all cell
     */
    void updateCell();
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
