#ifndef PositionController_h
#define PositionController_h

#include "Maze.hpp"
#include "Direction.hpp"
#include "Position.hpp"

class PositionController
{
private:
    Position pos; /**< current position of the mouse */
    Direction dir; /**< direction of the mouse */
    void init(int row, int col, Direction dirTo);

public:
    PositionController(int row, int col, Direction dirTo);
    PositionController(Position pos, Direction dirTo);
    PositionController();
    virtual void turnRight();
    virtual void turnLeft();
    virtual int goForward();

    inline void setDir(Direction dirToSet)
    {
        dir = dirToSet;
    }

    inline void setPos(Position posToSet)
    {
        pos = posToSet;
    }

    inline Direction getCurrentDir()
    {
        return dir;
    }
    inline Position getCurrentPos()
    {
        return pos;
    }
    Position getNextPos(Direction dirTo);

    inline Position getNextPos()
    {
        return PositionController::getNextPos(dir);
    }

    Direction getNextDir(Position posTo);

    inline Direction getNextDir(PositionController posTo)
    {
        return getNextDir(posTo.getCurrentPos());
    }
    bool operator==(PositionController& rVal)
    {
        return (getCurrentPos() == rVal.getCurrentPos());
    }

    void print(PositionController obj);
};

#endif
