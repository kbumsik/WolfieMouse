/*
 * FinderInterface.hpp
 *
 *  Created on: Mar 5, 2017
 *      Author: vagrant
 */

#ifndef FINDERINTERFACE_HPP_
#define FINDERINTERFACE_HPP_

#include "Wall.hpp"
#include "Direction.hpp"
#include "PositionController.hpp"

class FinderInterface
{
public:
    /* Examine if there is a wall or not.
     * It may look like @row and @col is not needed for @RealMouse, however, we 
     * can use them to see if the @RealMouse's sensor actually can reach that cell.
     * if the sensor is considered it cannot reach that wall, it will return @Wall::unknown.
     * @mousePos is not important for @SimulMouse but it is needed for @RealMouse
     * to decide which sensor to use. */  
    virtual Wall examineWall(int row, int col, Direction wallDir, PositionController &mousePos) = 0;
};

#endif /* FINDERINTERFACE_HPP_ */
