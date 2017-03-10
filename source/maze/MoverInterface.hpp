/*
 * MovingInterface.hpp
 *
 *  Created on: Mar 5, 2017
 *      Author: vagrant
 */

#ifndef MOVERINTERFACE_HPP_
#define MOVERINTERFACE_HPP_

#include "Direction.hpp"
#include "PositionController.hpp"

class MoverInterface
{
protected:
public:
    virtual void moveTo(int row, int col, Direction destDir, PositionController &mousePos) = 0;
};

#endif /* MOVERINTERFACE_HPP_ */
