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
/// For AS5304 & AS5000-MR20-44
// #define MEASURE_ENCODER_DEFAULT     10000
// #define MEASURE_STEPS_PER_REV       3520
// #define MEASURE_STEPS_PER_CELL      4800 //4750
// #define MEASURE_STEPS_90DEG_CW      1600
// #define MEASURE_STEPS_90DEG_CCW     1625

/// For AS5306 & AS5000-MR12-72
#define MEASURE_ENCODER_DEFAULT     20000
#define MEASURE_STEPS_PER_REV       5760
#define MEASURE_STEPS_PER_CELL      7875
#define MEASURE_STEPS_90DEG_CW      2618
#define MEASURE_STEPS_90DEG_CCW     2659
#define MEASURE_STEPS_SMOOTH_L_LEFT     4376
#define MEASURE_STEPS_SMOOTH_L_RIGHT    9258
#define MEASURE_STEPS_SMOOTH_R_LEFT     9258
#define MEASURE_STEPS_SMOOTH_R_RIGHT    4376
#define MEASURE_STEPS_BACK_TO_START_CENTER 1500 //< start is from 3.7cm

/**
 * Range finder measurements
 */
// #define MEASURE_RANGE_L_DETECT	180
// #define MEASURE_RANGE_R_DETECT  230
// #define MEASURE_RANGE_L_MIDDLE  353
// #define MEASURE_RANGE_R_MIDDLE  485
// #define MEASURE_RANGE_F_DETECT  450


#define MEASURE_RANGE_L_WALL_DETECT 600 
#define MEASURE_RANGE_L_MIN_DETECT	660
#define MEASURE_RANGE_L_M_DETECT    1150
#define MEASURE_RANGE_L_MAX_DETECT	1450

#define MEASURE_RANGE_R_WALL_DETECT 850 
#define MEASURE_RANGE_R_MIN_DETECT	900
#define MEASURE_RANGE_R_M_DETECT	1550
#define MEASURE_RANGE_R_MAX_DETECT	1700

#define MEASURE_RANGE_R_OFFSET 300 /* the offset between left and right sensor when mouse in the middle of cell */

#define MEASURE_RANGE_F_FAR_DETECT   310
#define MEASURE_RANGE_F_NEAR_DETECT  3600
// L:1836, 1465
// R

// F: 310, 375
// F_near 3789


/**
 * PID control
 */
#define CONFIG_DEFAULT_KP	0.9
#define CONFIG_DEFAULT_KD	0.2

#endif /* CONFIG_MEASUREMENTS_H_ */
