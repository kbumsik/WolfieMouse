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
    /* Constructors */
    PositionController(int row, int col, Direction dirTo);
    PositionController(Position pos, Direction dirTo);
    PositionController();
    /* Moving commends */
    virtual void turnRight();
    virtual void turnLeft();
    virtual int goForward();
    /* Direction getter and setters */
    Direction getNextDir(Position posTo);
    
    inline Direction getCurrentDir()
    {
        return dir;
    }

    inline Direction getNextDir(PositionController posTo)
    {
        return getNextDir(posTo.getCurrentPos());
    }

    inline void setDir(Direction dirToSet)
    {
        dir = dirToSet;
    }
    /* Position getter and setters */
    Position getNextPos(Direction dirTo);

    inline Position getCurrentPos()
    {
        return pos;
    }

    inline Position getNextPos()
    {
        return getNextPos(dir);
    }

    inline void setPos(Position posToSet)
    {
        pos = posToSet;
    }
    /* just printer of debugging */
    void print(PositionController obj);
    /* Comparation operator overloading */
    bool operator==(PositionController& rVal)
    {
        return (getCurrentPos() == rVal.getCurrentPos());
    }
};

#endif
