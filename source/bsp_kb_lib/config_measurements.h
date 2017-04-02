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
#define CONFIG_CNT_PER_REV      220
#define CONFIG_CNT_PER_CELL     360.1448
#define CONFIG_CNT_PER_90_DEG 	223.053
#define CONFIG_LEN_PER_CNT      0.49979

/**
 * Range finder measurements
 */
#define CONFIG_DetectRange	180	/* FIXME: change this */
#define CONFIG_LeftCenter	100
#define CONFIG_RightCenter	100
#define CONFIG_FrontWallRange 52
#define CONFIG_RangeOffset	0	/* the offset between left and right sensor when mouse in the middle of cell */


/**
 * PID control
 */
#define kP	0.9
#define kD	0.2

#endif /* CONFIG_MEASUREMENTS_H_ */
