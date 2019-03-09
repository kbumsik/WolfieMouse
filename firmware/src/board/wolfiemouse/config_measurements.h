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

/**
 * Range finder measurements
 */
#define MEASURE_RANGE_L_WALL_DETECT 600 
#define MEASURE_RANGE_R_WALL_DETECT 850 
#define MEASURE_RANGE_F_FAR_DETECT   310
#define MEASURE_RANGE_F_NEAR_DETECT  3600


/**
 * PID control
 */
#define CONFIG_DEFAULT_KP	0.9
#define CONFIG_DEFAULT_KD	0.2

#endif /* CONFIG_MEASUREMENTS_H_ */
