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

/* for vRangeFinderTask() */

/* Global Variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Task Handlers */
TaskHandle_t xBlinkyHandle;
TaskHandle_t xScanInputHandle;

/* Queue Handlers */
QueueHandle_t quUARTReceive;

/* Global function prototypes ------------------------------------------------*/
void Error_Handler(void);

/* Private function prototypes -----------------------------------------------*/
#ifdef __cplusplus
extern "C"{
#endif

void vBlinkyTask(void *pvParameters);
void vScanInputTask(void *pvParameters);
void vRangeFinderTask(void *pvParameters);

static void MX_I2C1_Init(void);

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
  xTaskCreate(vScanInputTask,
                "Scan",
                configMINIMAL_STACK_SIZE+1500,
                NULL,
                configMAX_PRIORITIES-1,
                &xScanInputHandle);
                */
  xTaskCreate(vRangeFinderTask,
                  "Range",
                  configMINIMAL_STACK_SIZE+2500,
                  NULL,
                  configMAX_PRIORITIES-1,
                  &xScanInputHandle);
  /* USER CODE BEGIN RTOS_QUEUES */
  /* definition and creation of xQueueUARTReceive */
  quUARTReceive = xQueueCreate(confUART_RECEIVE_QUEUE_LENGTH, /* length of queue */
                              sizeof(uint8_t)*confUART_RECEIVE_BUFFER_SIZE); /* size in byte of each item */
  /* USER CODE END RTOS_QUEUES */

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
      vLED_0_Toggle();
      /* Call this Task explicitly every 50ms ,NOT Delay for 50ms */
      vTaskDelayUntil(&xLastWakeTime, (200/portTICK_RATE_MS));
    }

  /* It never goes here, but the task should be deleted when it reached here */
  vTaskDelete(NULL);
}

/* vScanInputTask Task function */
void vScanInputTask(void *pvParameters)
{
  uint32_t uwCount = 1;
  int32_t swCheck = 0;
  int swInput = 0;
  char pcStr[30];

  /* Infinite loop */
  for (;;)
  {
    printf("ready\r\n");
    /* Set Blinky task less than this task, meaning stop blinking */
    //vTaskPrioritySet(xBlinkyHandle, uxPriority-1);
    swCheck = scanf("%d", &swInput);
    if (swCheck != 1)
    {
      printf("Wrong Input!: ");
      // flush the buffer
      scanf("%s", pcStr);
      printf("%s\r\n", pcStr);
    }
    else
    {
      printf("%d\r\n", swInput);
      /* Print the Idle count */
      printf("count=%d\r\n", uwCount++);
    }

    /* Set Blinky back */
    //vTaskPrioritySet(xBlinkyHandle, uxPriority);
    /* Set the task into blocking mode for 1000ms */
    /* The task becomes ready state after 1000ms */
    vTaskDelay(1000 / portTICK_RATE_MS);
  }

  /* It never goes here, but the task should be deleted when it reached here */
  vTaskDelete(NULL);
}

/************************************************************************
 * VL6180 codes
 */
#ifdef __cplusplus
extern "C"{
#endif

extern I2C_HandleTypeDef hi2c1; /* located in tca9545.h */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "vl6180x_api.h"
#include "x-nucleo-6180xa1.h"

#ifdef DEBUG
#include "diag/trace.h"
#define debug(msg, ...)   trace_printf(msg,__VA_ARGS__)
#define trace_warn(msg,...) trace_printf("W %s %d" msg "\n", __func__, __LINE__, __VA_ARGS__)
#else
#define debug(msg, ...)  (void)0
#endif

#define DigitDisplay_ms     1 /* ms each digit is kept on */
#define PressBPSwicthTime   1000 /* if user keep bp press more that this mean swicth mode else rooll over use c&se in mode */

void WaitMilliSec(int ms);

/**
 * VL6180x CubeMX F401 i2c porting implementation
 */

#define theVL6180xDev   0x52    // what we use as "API device
#define i2c_bus      (&hi2c1)
#define def_i2c_time_out 100
#if VL6180x_SINGLE_DEVICE_DRIVER
int VL6180x_I2CWrite(VL6180xDev_t addr, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Transmit(i2c_bus, addr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }
    return status;
}

int VL6180x_I2CRead(VL6180xDev_t addr, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Receive(i2c_bus, addr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }

    return status;
}
#else
int VL6180x_I2CWrite(VL6180xDev_t dev, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Transmit(i2c_bus, dev->I2cAddr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }
    return status;
}

int VL6180x_I2CRead(VL6180xDev_t dev, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Receive(i2c_bus, dev->I2cAddr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }

    return status;
}

#endif

/**
 * platform and application specific for VL6180x_Shield
 */
void XNUCLEO6180XA1_WaitMilliSec(int n) {
    WaitMilliSec(n);
}

volatile int IntrFired = 0;

void XNUCLEO6180XA1_UserIntHandler(void) {
    IntrFired++;
}

/**
 * DISPLAY public
 */
/***************  DISPLAY PUBLIC *********************/
const char *DISP_NextString;
/***************  DISPLAY PRIVATE *********************/
static char DISP_CurString[10];
static int DISP_Loop = 0;
uint32_t TimeStarted; /* various display and mode delay starting time */

#ifdef TEMP  /** TODO: Delete */
void DISP_ExecLoopBody(void) {
    if (DISP_NextString != NULL) {
        strncpy(DISP_CurString, DISP_NextString, sizeof(DISP_CurString) - 1);
        DISP_CurString[sizeof(DISP_CurString) - 1] = 0;
        DISP_NextString = NULL;
    }
    XNUCLEO6180XA1_DisplayString(DISP_CurString, DigitDisplay_ms);
    DISP_Loop++;
}
#else
void DISP_ExecLoopBody(void) {
    if (DISP_NextString != NULL) {
        strncpy(DISP_CurString, DISP_NextString, sizeof(DISP_CurString) - 1);
        DISP_CurString[sizeof(DISP_CurString) - 1] = 0;
        DISP_NextString = NULL;
    }
    printf("%.4s\n", DISP_CurString);
    DISP_Loop++;
}
#endif

/**
 * Nucleo board specific
 *
 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    XNUCLEO6180XA1_EXTI_CallBackHandle(GPIO_Pin);
}

volatile uint32_t g_TickCnt;
void HAL_SYSTICK_Callback(void){
    g_TickCnt++;
}

#define BSP_BP_PORT GPIOC
#define BSP_BP_PIN  GPIO_PIN_13
int BSP_GetPushButton(void) {
    GPIO_PinState state;
    state = HAL_GPIO_ReadPin(BSP_BP_PORT, BSP_BP_PIN);
    return state;
}

void SetDisplayString(const char *msg) {
    DISP_NextString = msg;
}

void WaitMilliSec(int ms) {
    HAL_Delay(ms); /* it's milli sec  cos we do set systick to 1KHz */
}

/**
 * call in the main loop
 * when running under debugger it enable doing direct vl6180x reg access
 * After  breaking at entrance
 * change  the the local index/data and cmd variable to do what needed
 * reg_cmd -1 wr byte -2wr word -3 wr dword
 * reg_cmd 1 rd byte 2 rd word 3 rd dword
 * step to last statement before return and read variable to get rd result exit
 */
void debug_stuff(void) {
    int reg_cmd = 0;
    static uint32_t reg_data;
    static uint16_t reg_index;

    if (reg_cmd) {
        switch (reg_cmd) {
            case -1:
                VL6180x_WrByte(theVL6180xDev, reg_index, reg_data);
                debug("Wr B 0x%X = %d", reg_index, (int)reg_data);
                break;
            case -2:
                VL6180x_WrWord(theVL6180xDev, reg_index, reg_data);
                debug("Wr W 0x%X = %d", reg_index, (int) reg_data);
                break;

            case -3:
                VL6180x_WrDWord(theVL6180xDev, reg_index, reg_data);
                debug("WrDW 0x%X = %d", reg_index, (int)reg_data);
                break;

            case 1:
                reg_data = 0;
                VL6180x_RdByte(theVL6180xDev, reg_index, (uint8_t*) &reg_data);
                debug("RD B 0x%X = %d", reg_index, (int)reg_data);
                break;
            case 2:
                reg_data = 0;
                VL6180x_RdWord(theVL6180xDev, reg_index, (uint16_t*) &reg_data);
                debug("RD W 0x%X = %d", reg_index, (int)reg_data);
                break;

            case 3:
                VL6180x_RdDWord(theVL6180xDev, reg_index, &reg_data);
                debug("RD DW 0x%X = %d", reg_index, (int)reg_data);
                break;
            default:
                debug("Invalid command %d", reg_cmd);
                /* nothing to do*/
                ;
        }
    }
}

/**
 * When button is already pressed it Wait for user to release it
 * if button remain pressed for given time it return true
 * These is used to detect mode switch by long press on blue Push Button
 *
 * As soon as time is elapsed -rb- is displayed  to let user know order
 * the  request to switch mode is taken into account
 *
 * @return True if button remain pressed more than specified time
 */
int PusbButton_WaitUnPress(void) {
    uint32_t TimeStarted = HAL_GetTick();
    uint32_t tick;

    while (!BSP_GetPushButton()) {
        ; /* debounce */
        DISP_ExecLoopBody();
        tick = HAL_GetTick();
        if (-TimeStarted > PressBPSwicthTime) {
            SetDisplayString(" rb ");
        }
    }
    return tick - TimeStarted > PressBPSwicthTime;
}

void Sample_FreeRunningRanging(void);
void Sample_SimpleRanging(void);
void Sample_SimpleAls(void);
uint8_t Sample_OffsetCalibrate(void);
void Sample_XTalkCalibrate(int initDevice);
void Sample_AlternateRangeAls(void);
void Sample_Interrupt(void);

void vRangeFinderTask(void *pvParameters) {
    uint8_t offset;

    /* MCU Configuration----------------------------------------------------------*/


    /* Initialize all configured peripherals */
    MX_I2C1_Init();

    tca9545_init();
    tca9545_set(Channel_0);

    /* these almost just redo what already done just above by CubeMx Init */
    XNUCLEO6180XA1_GPIO_Init();
    XNUCLEO6180XA1_I2C1_Init(&hi2c1);

    while (1) {

        Sample_SimpleRanging();

        Sample_FreeRunningRanging();

        Sample_SimpleAls();

        Sample_AlternateRangeAls();

        offset = Sample_OffsetCalibrate();

        Sample_XTalkCalibrate(offset);

        Sample_Interrupt();
    }
    /* USER CODE END 3 */

}

void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
  HAL_I2C_Init(&hi2c1);

}
#ifdef __cplusplus
}
#endif

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
