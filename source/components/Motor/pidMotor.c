/*
 * pidMotor.c
 *
 *  Created on: Apr 16, 2016
 *      Author: Bumsik Kim
 */


#include "config.h"
#include "pidMotor.h"
#include "config_measurements.h"
#include "Motor.h"
#include "range_finder.h"
#include "encoder.h"

extern SemaphoreHandle_t muRange;
extern int range[4];
extern int motorSpeed[2];

void pidMotorInit(void){

}
void pidMotorStart(eMotorChannel_t channel){

}
void pidMotorStop(eMotorChannel_t channel){

}
void pidMotorSetSpeed(eMotorChannel_t channel, int speed){

}
void pidMotorUpdateToCenter(void){
	int leftSensor, rightSensor;
	int errorP, errorD;
	int totalError;
	static int oldErrorP = 0;

	/* TODO: protector */

    xSemaphoreTake(muRange, portMAX_DELAY); /* Take Mutext */

	leftSensor = range[left_side];
	rightSensor = range[right_side];

    xSemaphoreGive(muRange); /* Release mutex */


    if( (leftSensor < configDetectRange)&&(rightSensor < configDetectRange) )//has both walls
    {
    	/* ccw direction is positive */
        errorP = (rightSensor - leftSensor); /*63 is the offset between left and
        										right sensor when mouse in the middle of cell */
        errorD = (errorP - oldErrorP);
    }
    else if((leftSensor < configDetectRange))//only has left wall
    {
        errorP = 2 * (configLeftCenter - leftSensor);
        errorD = errorP - oldErrorP;
    }
    else if((rightSensor < configDetectRange))//only has right wall
    {
        errorP = 2 * (rightSensor - configRightCenter);
        errorD = errorP - oldErrorP;
    }
    else if( (leftSensor > configDetectRange) && (rightSensor > configDetectRange))//no wall, use encoder or gyro
    {
        errorP = 0;//(leftEncoder – rightEncoder*1005/1000)*3;
        errorD = 0;
    }
    totalError = kP * errorP + kD * errorD;
    oldErrorP = errorP;
    swMotorSetSpeed(motorSpeed[left] - totalError, left);
    swMotorSetSpeed(motorSpeed[right] + totalError, right);
}


void pidMotorMoveFor1Cell(int speed)
{
	int start_count;
	start_count = getLeftEncCount();
	eMotorStop(all);
	vMotorGoForward();

	motorSpeed[left] = speed;
	motorSpeed[right] = speed;
	pidMotorUpdateToCenter();
	eMotorStart(all);
	while ( (getLeftEncCount() - start_count)  < ((int)configCntPerCell) )
	{
		pidMotorUpdateToCenter();
	}
	eMotorStop(all);

}
void pidMotorRotateFor90Degree(int speed)
{

}
void pidMotorMoveForHalfCell(int speed)
{

}
void pidMotorRotateFor180Degree(int speed)
{

}
