/*
 * control.c
 *
 *  Created on: Jan 30, 2016
 *      Author: Bumsik Kim
 */
#include "control.h"

float fControlPIDCompute(float setpoint, float input, controlPid_t *obj)
{
   float output;
   /*How long since we last calculated*/
   uint32_t now = HAL_GetTick();
   float timeChange = (float)(now - obj->lastTime);

   /*Compute all the working error variables*/
   float error = setpoint - input;
   obj->errSum += (error * timeChange);
   float dErr = (error - obj->lastErr) / timeChange;

   /*Compute PID Output*/
   output = obj->kp * error + obj->ki * obj->errSum + obj->kd * dErr;

   /*Remember some variables for next time*/
   obj->lastErr = error;
   obj->lastTime = now;
   return output;
}

void vControlPIDSetTunings(float Kp, float Ki, float Kd, controlPid_t *obj)
{
  obj->kp = Kp;
  obj->ki = Ki;
  obj->kd = Kd;
}
