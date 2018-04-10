
#ifndef GREAT_H_
#define GREAT_H_

#include "FinderInterface.hpp"
#include "MoverInterface.hpp"
#include "Maze.hpp"
#include "IOInterface.hpp"


class RealMouse: public FinderInterface, public MoverInterface
{
public:
    // Constructor
    RealMouse();
    /* Overriding FinderInterface methods */
    Wall examineWall(int row, int col, Direction wallDir, PositionController &mousePos) override;
    Wall examineWall(Position pos, Direction wallDir, PositionController &mousePos)
    {
            return examineWall(pos.row, pos.col, wallDir, mousePos);
    }
    /* Overriding MoverInterface methods */
    void moveTo(int row, int col, Direction destDir, PositionController &mousePos) override;
    void moveTo(Position pos, Direction destDir, PositionController &mousePos)
    {
        return moveTo(pos.row, pos.col, destDir, mousePos);
    }
    void rotateTo(Direction destDir, PositionController &mousePos) override;
};

#endif
