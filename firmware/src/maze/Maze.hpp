#ifndef Maze_h
#define Maze_h

#include <vector>

#include "Cell.hpp"
#include "common_maze.h"
#include "config_maze.h"
#include <stdio.h>
#include "Direction.hpp"
#include "Position.hpp"
#include "Wall.hpp"
#include "MazeIO.hpp"
#include "IOInterface.hpp"


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
protected:
    // Maze printer
    MazeIO mazeIO;
public:
    Position startPos;
    std::vector<Position> goalPos;
    struct StringMaze {
        /* +1 is null character */
        char buf[CONFIG_MAZE_IO_BUFFER_SIZE + 1];
    };

    /* Constructors */
    Maze(IOInterface *fileIO, IOInterface *printIO);
    Maze(char* filename, IOInterface *fileIO, IOInterface *printIO);
    Maze(StringMaze *stringMaze, IOInterface *fileIO, IOInterface *printIO);
    /* Related to wall */
    Wall getWall(int row, int col, Direction dir);
    inline Wall getWall(Position pos, Direction dir);
    int setWall(int row, int col, Direction dir, Wall status);
    inline int setWall(Position pos, Direction dir, Wall status);
    /* Related to Cell */
    Cell getCell(int row, int col);
    inline Cell getCell(Position pos);
    int updateCell(int row, int col);
    inline int updateCell(Position pos);
    void updateCell();
    /* getter and setter of Distance of cell */
    inline int getDistance(int row, int col);
    inline int getDistance(Position pos);
    inline int getDistancePrint(int row, int col);
    inline int getDistancePrint(Position pos);
    inline int setDistance(int row, int col, int dis);
    inline int setDistance(Position pos, int dis);
    /* IO Related */
    void readMazeFromFile(char* fileName);
    void readMazeFromString(StringMaze *stringMaze);
    virtual void printMaze();
    void saveMazeFile(char* fileName);
};

/*******************************************************************************
 * Inline function definition
 ******************************************************************************/
/* Related to wall */
inline Wall Maze::getWall(Position pos, Direction dir)
{
    return getWall(pos.row, pos.col, dir);
}

inline int Maze::setWall(Position pos, Direction dir, Wall status)
{
    return setWall(pos.row, pos.col, dir, status);
}

/* Related to Cell */
inline Cell Maze::getCell(Position pos)
{
    return getCell(pos.row, pos.col);
}

inline int Maze::updateCell(Position pos)
{
    return updateCell(pos.row, pos.col);
}

/* getter and setter of Distance of cell */
inline int Maze::getDistance(int row, int col)
{
	if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
	    return COMMON_MAZE_ERROR;
	}
    return cell[row][col].distance;
}

inline int Maze::getDistance(Position pos)
{
    return getDistance(pos.row, pos.col);
}

inline int Maze::getDistancePrint(int row, int col)
{
    if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
        printf("invalid cell!\n");
        return COMMON_MAZE_ERROR;
    }
    return cell[row][col].distance;
}

inline int Maze::getDistancePrint(Position pos)
{
    return getDistancePrint(pos.row, pos.col);
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

inline int Maze::setDistance(Position pos, int dis)
{
    return setDistance(pos.row, pos.col, dis);
}

#endif
