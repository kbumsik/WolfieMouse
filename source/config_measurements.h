/*
 * config_measurements.h
 *
 *  Created on: Apr 16, 2016
 *      Author: Bumsik Kim
 */

#ifndef CONFIG_MEASUREMENTS_H_
#define CONFIG_MEASUREMENTS_H_

/**
 * Encoder measurements
 */
#define configCntPerRev		220
#define configCntPerCell  	360.1448
#define configCntPer90Deg 	223.053
#define configLenPerCnt		0.49979

/**
 * Range finder measurements
 */
#define configDetectRange	180	/* FIXME: change this */
#define configLeftCenter	100
#define configRightCenter	100
#define configFrontWallRange 52
#define configRangeOffset	0	/* the offset between left and right sensor when mouse in the middle of cell */


/**
 * PID control
 */
#define kP	0.9
#define kD	0.2

#endif /* CONFIG_MEASUREMENTS_H_ */
