#ifndef PositionController_h
#define PositionController_h

#include "Direction.hpp"
#include "Position.hpp"

/*******************************************************************************
 * Class Declaration
 ******************************************************************************/
class PositionController
{
private:
    Position pos; /**< current position of the mouse */
    Direction dir; /**< direction of the mouse */

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
    inline Direction getCurrentDir();
    Direction getNextDir(Position posTo);
    inline Direction getNextDir(PositionController posTo);
    inline void setDir(Direction dirToSet);
    /* Position getter and setters */
    inline Position getCurrentPos();
    Position getNextPos(Direction dirTo);
    inline Position getNextPos();
    inline void setPos(Position posToSet);
    /* just printer of debugging */
    void print(PositionController obj);
    /* Comparator operator overloading */
    bool operator==(PositionController& rVal)
    {
        return (getCurrentPos() == rVal.getCurrentPos());
    }
};

/*******************************************************************************
 * Inline function definition
 ******************************************************************************/
inline Direction PositionController::getCurrentDir()
{
    return dir;
}

inline Direction PositionController::getNextDir(PositionController posTo)
{
    return getNextDir(posTo.getCurrentPos());
}

inline void PositionController::setDir(Direction dirToSet)
{
    dir = dirToSet;
}

inline Position PositionController::getCurrentPos()
{
    return pos;
}

inline Position PositionController::getNextPos()
{
    return getNextPos(dir);
}

inline void PositionController::setPos(Position posToSet)
{
    pos = posToSet;
}

#endif
