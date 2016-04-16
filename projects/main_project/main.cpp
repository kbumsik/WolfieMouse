/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
//#define configDEBUG

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "main.h"
#include "board_init.h"
#include "fatfs.h"
#include <stdio.h>
#include "range_finder.h"
#include "Motor.h"
#include "encoder.h"

#include "pidMotor.h"

/* for vRangeFinderTask() */

/* Global Variables ----------------------------------------------------------*/
int rightSpeed = 0;
int leftSpeed = 0;
int diffSpeed = 0;
/* Private variables ---------------------------------------------------------*/
/* Task Handlers */
TaskHandle_t xBlinkyHandle;
TaskHandle_t xScanInputHandle;
TaskHandle_t xMainHandle;

/* Mutex Handlers */
SemaphoreHandle_t muRange;

/* Queue Handlers */
QueueHandle_t quUARTReceive;

/* ranges */
int range[4] = {255, 255, 255, 255};
int motorSpeed[2] = {0, 0};

/* Global function prototypes ------------------------------------------------*/
void Error_Handler(void);

/* Private function prototypes -----------------------------------------------*/
#ifdef __cplusplus
extern "C"{
#endif

void vBlinkyTask(void *pvParameters);
void vRangeFinderTask(void *pvParameters);
//void vEncoderTask(void *pvParameters);
void vMainTask(void *pvParameters);

#ifdef __cplusplus
}
#endif


int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface
   * and the Systick. */
  board_Init();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  muRange = xSemaphoreCreateMutex();
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  xTaskCreate(vBlinkyTask,			        /* Pointer to the function that implements the task */
		  	  "Blinky",						/* Text name for the task. This is to facilitate debugging only. It is not used in the scheduler */
		  	  configMINIMAL_STACK_SIZE,		/* Stack depth in words */
		  	  NULL,							/* Pointer to a task parameters */
		  	  1,		                    /* The task priority */
		  	  &xBlinkyHandle);                        /* Pointer of its task handler, if you don't want to use, you can leave it NULL */

  /*
  xTaskCreate(vRangeFinderTask,
                  "Range",
                  configMINIMAL_STACK_SIZE+500,
                  NULL,
                  configMAX_PRIORITIES-1,
                  &xScanInputHandle);
*/
  /*
  xTaskCreate(vEncoderTask,
                  "Encoder",
                  configMINIMAL_STACK_SIZE+500,
                  NULL,
                  configMAX_PRIORITIES-1,
                  &xScanInputHandle);
                  */
  xTaskCreate(vMainTask,
                    "Main",
                    configMINIMAL_STACK_SIZE+2500,
                    NULL,
                    configMAX_PRIORITIES-1,
                    &xMainHandle);
  /* USER CODE BEGIN RTOS_QUEUES */
  /* definition and creation of xQueueUARTReceive */
  quUARTReceive = xQueueCreate(confUART_RECEIVE_QUEUE_LENGTH, /* length of queue */
                              sizeof(uint8_t)*confUART_RECEIVE_BUFFER_SIZE); /* size in byte of each item */
  /* USER CODE END RTOS_QUEUES */

  /* Init components */
  eMotorInit();
  vMotorGoForward();

  encoder_init();


  /* Start scheduler */
  vTaskStartScheduler();
  /* NOTE: We should never get here as control is now taken by the scheduler */
  while (1)
  {
  }
}

/* vBlinkyTask function */
void vBlinkyTask(void *pvParameters)
{
  portTickType xLastWakeTime;
  /* Initialize xLastWakeTime for vTaskDelayUntil */
  /* This variable is updated every vTaskDelayUntil is called */
  xLastWakeTime = xTaskGetTickCount();

  for(;;)
    {
      vLED_1_Toggle();
      vLED_2_Toggle();
      vLED_3_Toggle();
      vLED_4_Toggle();
      /* Call this Task explicitly every 50ms ,NOT Delay for 50ms */
      vTaskDelayUntil(&xLastWakeTime, (200/portTICK_RATE_MS));
    }

  /* It never goes here, but the task should be deleted when it reached here */
  vTaskDelete(NULL);
}


void vRangeFinderTask(void *pvParameters) {

	  portTickType xLastWakeTime;
	  /* Initialize xLastWakeTime for vTaskDelayUntil */
	  /* This variable is updated every vTaskDelayUntil is called */
	  xLastWakeTime = xTaskGetTickCount();
	range_finder_init();

    while (1) {
        xSemaphoreTake(muRange, portMAX_DELAY); /* Take Mutext */
    	range[0] = range_finder_get(left_side);
    	range[1] = range_finder_get(right_side);
    	range[2] = range_finder_get(left_front);
    	range[3] = range_finder_get(right_front);
    	xSemaphoreGive(muRange);
        /* Call this Task explicitly every 50ms ,NOT Delay for 50ms */
        vTaskDelayUntil(&xLastWakeTime, (5/portTICK_RATE_MS));
    }
    /* USER CODE END 3 */

}




/*
void vEncoderTask(void *pvParameters) {

	  int count_l = 0, count_r = 0;
	  while (1) {
		  count_l = getLeftEncCount();
		  count_r = getRightEncCount();
	  }


}
*/

void vMainTask(void *pvParameters)
{


	vTaskDelay(1000);

	for (int speed = 500; speed < 10000; speed = speed + 50)
	{
		  swMotorSetSpeed(speed, all);
		  eMotorStart(all);
		  HAL_Delay(1000);
		  eMotorStop(all);

		  vMotorGoBackward();
		  swMotorSetSpeed(speed, all);
		  eMotorStart(all);
		  HAL_Delay(500);
		  eMotorStop(all);

		  vMotorTurnRight();
		  swMotorSetSpeed(speed, all);
		  eMotorStart(all);
		  HAL_Delay(500);
		  eMotorStop(all);

		  vMotorTurnLeft();
		  swMotorSetSpeed(speed, all);
		  eMotorStart(all);
		  HAL_Delay(500);
		  eMotorStop(all);
	}

	 // pidMotorMoveFor1Cell(85);

	while(1)
	{

	}
}


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  while(1)
  {

  }
}

#endif

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while(1)
  {
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
