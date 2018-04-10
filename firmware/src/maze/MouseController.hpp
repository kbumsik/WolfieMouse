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
    /* Vector destination */
    std::vector<Position> destinations;
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

    /*************** Floodfiil Algorithm Implementation ***********************/
    /* Algorithm solver */
    void getDistanceAllCell(void);
    void getShortestPath(void);
    /* Used in algorithm implementation */
    void initDistance(void);
    int getHighestNeighbouringDistance(int row, int col);
    /* Setting Direction */
    Direction getDirectionToGo(void);
    void setDirectionToGo(void);

    /*************** Interacting physical interfaces **************************/
    /**
     * @brief Scan walls around the mouse.
     * 
     * @return true     Any new wall updated 
     * @return false    No wall information updated
     */
    bool scanWalls(void);
    /**
     * @brief Move one cell toward the shortest path, with given machine state 
     *        and information.
     */
    void moveNextShortestCell(void);

    /* Moving methods */
    /* TODO: These are not yet physically move. check these moving methods */
    void turnRight(void) override;
    void turnLeft(void) override;
    int goForward(void) override;
    
    
    /** On development
     void movingCompleted();

     Cell* getNextCell();

     virtual void setWall(int row, int col, direction_e dir, wall_e status);
     virtual void updateCell(int row, int col);
     */
public:
    /*********************  Constructors **************************************/
    MouseController(char *filename, IOInterface *fileIO, IOInterface *printIO,
                    FinderInterface *finder, MoverInterface *mover);

    /*********************  Movement commands *********************************/
    /**
     * @brief Scan, update path, and move one cell
     * 
     * @param wait_func Blocking method before moving and after moving. Useful
     *                  for simulation but you may not want use it in the
     *                  competition. Can be NULL if not used.
     * @return true     When successful move
     * @return false    When move failed
     */
    bool scanAndMove(void (*wait_func)(MouseController *mouse));

    /*************************  Destination changers **************************/
    void makeRandomDest(unsigned int n);
    void makeStartAsDest(void);
    void makeGoalAsDest(void);

    /************* boolean functions for goal, start and destination **********/
    bool anyDestinationCellSearched(void);
    bool positionIsDestination(Position pos);
    bool isInDestinationCell(void);
    bool allDestinationsReached(void);
    bool isInGoal(void);
    bool isInStart(void);

    /*************** print information of stack used in the algorithm *********/
    void printMaze(void) override;
    void printPathStack(void);
    void printAvailablePositionStack(void);
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
