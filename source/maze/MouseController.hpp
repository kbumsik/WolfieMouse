#ifndef MouseController_h
#define MouseController_h

#include <vector>

#include "config_maze.h"
#include "Maze.hpp"
#include "PositionController.hpp"
#include "Queue.hpp"
#include "FinderInterface.hpp"
#include "MoverInterface.hpp"

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
	/* Helper objects for shortest path algorithm */
    Queue<PositionController> pathStack; /**< This is an assistant stack. When @getShortestPath invoked the path to the goal is constructed. */
    Queue<PositionController> availablePositionStack; /**< I don't even know what is this. */
    /* Interfaces */
    FinderInterface *finder;
    MoverInterface *mover;
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
    /* Used in algorithm implementation */
    void initDistance();
    int getHighestNeighbouringDistance(int row, int col);
    /* Setting Direction */
    Direction getDirectionToGo();
    void setDirectionToGo();
    /* Moving methods */
    void turnRight() override;
    void turnLeft() override;
    int goForward() override;
    /** On development
     void moveNextCell();
     void movingCompleted();

     Cell* getNextCell();

     virtual void setWall(int row, int col, direction_e dir, wall_e status);
     virtual void updateCell(int row, int col);
     */
    /* Vector destination */
    std::vector<Position> destinations;
public:
    /* Constructors */
    MouseController();
    MouseController(char *filename, IOInterface *fileIO, IOInterface *printIO,
                    FinderInterface *finder, MoverInterface *mover);
    /* Algorithm solver */
    void scanWalls(void);     // Detect and update walls while not moving.
    void getDistanceAllCell();
    void getShortestPath();
    void moveNextCell();
    void setUnsearchDes(int n);
    void setStartAsDes();
    void setGoalAsDes();
    /* boolean functions for goal, start and destination */
    bool anyDestinationCellSearched();
    bool positionIsDestination(Position pos);
    bool isInDestinationCell();
    bool allDestinationsReached();
    bool isInGoal();
    bool isInStart();
    /* print information of stack used in the algorithm */
    void printMaze() override;
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
