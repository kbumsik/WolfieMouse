/*
 * range_finder.h
 *
 *  Created on: Apr 15, 2016
 *      Author: Bumsik Kim
 */

#ifndef RANGE_FINDER_H_
#define RANGE_FINDER_H_

#include "tca9545.h"

#ifdef __cplusplus
extern "C"{
#endif

enum range_finder_channel{
	left_side = 0,
	right_side,
	left_front,
	right_front
};

void range_finder_init();
int range_finder_get(enum range_finder_channel channel);
void range_finder_update(int *range);

#ifdef __cplusplus
}
#endif

#endif /* RANGE_FINDER_H_ */
