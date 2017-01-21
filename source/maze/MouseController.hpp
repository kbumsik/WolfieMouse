#ifndef MouseController_h
#define MouseController_h

#include "config_maze.h"
#include "Maze.hpp"
#include "PositionController.hpp"
#include "Queue.hpp"

/*******************************************************************************
 * Class Declaration
 ******************************************************************************/
/**
 * @brief      This class it the top level of the mouse control.
 * @details    ===== MOUSE CONTROL PROCEDURE =====
 *  1. call @getDistanceAllCell
 *  2. call @getShortestPath
 *  3. call @moveNextCell
 */
class MouseController: public Maze, public PositionController
{
private:
    Queue<PositionController> pathStack; /**< This is an assistant stack. When @getShortestPath invoked the path to the goal is cunstructed. */
    Queue<PositionController> availablePositionStack; /**< I don't even know what is this. */

    /* Distance getter and setters */ 
    inline int getDis(int row, int col);
    inline int getDis(Position pos);
    inline int getDis(PositionController pos);
    inline int getNextDis(PositionController pos, Direction dirTo);
    inline int getNextDis(PositionController pos);
    inline int setDis(int row, int col, int dis);
    inline void setDis(Position pos, int dis);
    /* Cell getter and setter */
    Cell getCell(Position pos);
    void updateCell();
    /* Used in algorithm implementation */
    void initDistance();
    int getHighestNeighbouringDistance(int row, int col);
    /* Setting Direction */
    Direction getDirectionToGo();
    void setDirectionToGo();
    /** On development
     void moveNextCell();
     void movingCompleted();

     Cell* getNextCell();

     virtual void turnRight();
     virtual void turnLeft();
     virtual void goForward();

     virtual void setWall(int row, int col, direction_e dir, wall_e status);
     virtual void updateCell(int row, int col);
     */
public:
    /* Constructors */
    MouseController();
    MouseController(char *filename);
    /* Algorithm solver */
    void getDistanceAllCell();
    void getShortestPath();
    void moveNextCell();
    /* Goad and Start */
    bool isInGoal();
    bool isInStart();
    /* print information of stack used in the algorithm */
    void printPathStack();
    void printAvailablePositionStack();
};

/*******************************************************************************
 * Inline function definition
 ******************************************************************************/
inline int MouseController::getDis(int row, int col)
{
    return Maze::getDistance(row, col);
}

inline int MouseController::getDis(Position pos)
{
    return Maze::getDistance(pos.row, pos.col);
}

inline int MouseController::getDis(PositionController pos)
{
    Position position = pos.getCurrentPos();
    return Maze::getDistance(position.row, position.col);
}

inline int MouseController::getNextDis(PositionController pos, Direction dirTo)
{
    PositionController tmp = PositionController(pos.getCurrentPos(), dirTo);
    return getDis(tmp.getNextPos());
}

inline int MouseController::getNextDis(PositionController pos)
{
    return getDis(pos.getNextPos());
}

inline int MouseController::setDis(int row, int col, int dis)
{
    return Maze::setDistance(row, col, dis);
}

inline void MouseController::setDis(Position pos, int dis)
{
    setDis(pos.row, pos.col, dis);
}

#endif
