/*
 * SimulMouse.hpp
 *
 *  Created on: Mar 5, 2017
 *      Author: vagrant
 */

#ifndef SIMULMOUSE_HPP_
#define SIMULMOUSE_HPP_

#include "FinderInterface.hpp"
#include "MoverInterface.hpp"

class SimulMouse: public FinderInterface, public MoverInterface
{
public:
	// Constructor
	SimulMouse();
    /* Overriding FinderInterface methods */
	void find(void) override;
    /* Overriding MoverInterface methods */
	void move(void) override;
};

#endif /* SIMULMOUSE_HPP_ */
