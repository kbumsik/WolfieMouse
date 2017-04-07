/*
 * config_measurements.h
 *
 *  Created on: Apr 16, 2016
 *      Author: Bumsik Kim
 */

#ifndef CONFIG_MEASUREMENTS_H_
#define CONFIG_MEASUREMENTS_H_

/**
 * # About Micromouse Porject's measurements and speed settings.
 *  We measure speed every 1ms, therefore, for performance we need to use
 *  mm/ms other than m/s.
 *  Also, using mm/ms needs addtional mutiplication operation. So we might need 
 *  to consider using Counts/ms.
 *
 * # Basic facts
 *  1m/s = 1mm/ms (Yeah!)
 *  pulse/rev (from datasheet) = 3520 steps / rev
 *  Wheel diameter = 300mm
 *        perimeter = 300 * pi = 942.478mm
 *  
 *  Size of one cell in the maze = 18cm = 180mm
 *
 * # Motor profile:
 *  1:5 gearmotor
 *  speed @ 10% power, free space = 16~20 steps / ms
 *  speed @ 30% power, free space = ~70 steps / ms
 *  speed @ 50% power, free space = 120~125 steps / ms
 *  speed @ 70% power, free space = ~175 steps / ms
 */
/**
 * Encoder measurements
 */
#define CONFIG_CNT_PER_REV      3520
#define CONFIG_CNT_PER_CELL     360.1448
#define CONFIG_CNT_PER_90_DEG 	223.053
#define CONFIG_LEN_PER_CNT      0.49979

/**
 * Range finder measurements
 */
#define MEASURE_RANGE_L_DETECT	1500	/* FIXME: change this */
#define MEASURE_RANGE_R_DETECT  1500
#define MEASURE_RANGE_L_MIDDLE  1800    /* FIXME: change this */
#define MEASURE_RANGE_R_MIDDLE  1800
#define CONFIG_FrontWallRange 52
#define CONFIG_RangeOffset	0	/* the offset between left and right sensor when mouse in the middle of cell */


/**
 * PID control
 */
#define CONFIG_DEFAULT_KP	0.9
#define CONFIG_DEFAULT_KD	0.2

#endif /* CONFIG_MEASUREMENTS_H_ */
