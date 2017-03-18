/*
 * system_config_tmplete.c
 *
 *  Created on: Oct 20, 2016
 *      Author: Bumsik Kim
 */

#include "kb_common_header.h"
#include "kb_module_config.h"
#include "kb_tick.h"
#include "system_config.h"
#include "faults.h"


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
	  /* User can add his own implementation to report the HAL error return state */
	  while(1)
	  {
	  }
}

void system_init(void)
{
	  // Initialise the HAL Library; it must be the first function
	  // to be executed before the call of any HAL function.
	  HAL_Init();

	  // Set NVIC
	  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);

	  /* System interrupt init*/
	  /* MemoryManagement_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	  /* BusFault_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	  /* UsageFault_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	  /* SVCall_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
	  /* DebugMonitor_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
	  /* PendSV_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
	  /* SysTick_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	  // Enable HSE Oscillator and activate PLL with HSE as source
	  SystemClock_Config();

	  // Call the CSMSIS system clock routine to store the clock frequency
	  // in the SystemCoreClock global RAM location.
	  SystemCoreClockUpdate();
	  // update f_cpu_MHz too
	  kb_tick_update_f_cpu_mhz();

	  // Enable fault calls
	  enable_faults();

	  // init timer
	  kb_tick_init();
}


/** System Clock Configuration
*/
void SystemClock_Config(void)
{

	  RCC_OscInitTypeDef RCC_OscInitStruct;
	  RCC_ClkInitTypeDef RCC_ClkInitStruct;

	    /**Configure the main internal regulator output voltage
	    */
	  __HAL_RCC_PWR_CLK_ENABLE();

	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	    /**Initializes the CPU, AHB and APB busses clocks
	    */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	  RCC_OscInitStruct.HSICalibrationValue = 16;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	  RCC_OscInitStruct.PLL.PLLM = 8;
	  RCC_OscInitStruct.PLL.PLLN = 120;
	  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	  RCC_OscInitStruct.PLL.PLLQ = 2;
	  RCC_OscInitStruct.PLL.PLLR = 2;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }

	    /**Activate the Over-Drive mode
	    */
	  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	  {
	    Error_Handler();
	  }

	    /**Initializes the CPU, AHB and APB busses clocks
	    */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	  {
	    Error_Handler();
	  }

	    /**Configure the Systick interrupt time
	    */
	  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	    /**Configure the Systick
	    */
	  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	  /* SysTick_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PA2   ------> USART2_TX
     PA3   ------> USART2_RX
*/
#include "kb_gpio.h"
#include "kb_terminal.h"
void peripheral_init(void)
{
	  kb_gpio_init_t GPIO_InitStruct;

	  /*Configure GPIO pin : B1_Pin */
	  //GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  //GPIO_InitStruct.Pull = GPIO_NOPULL;
	  //GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  //kb_gpio_init(B1_PORT, B1_PIN, &GPIO_InitStruct);

	  /*Configure UART */
	  kb_terminal_init();

	  /*Configure GPIO pin : LED1_Pin */
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  kb_gpio_init(LED1_PORT, LED1_PIN, &GPIO_InitStruct);
	  kb_gpio_init(LED2_PORT, LED2_PIN, &GPIO_InitStruct);
	  kb_gpio_init(LED3_PORT, LED3_PIN, &GPIO_InitStruct);
	  kb_gpio_init(LED4_PORT, LED4_PIN, &GPIO_InitStruct);

	  /*Configure GPIO pin Output Level */
	  kb_gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
	  kb_gpio_set(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
	  kb_gpio_set(LED3_PORT, LED3_PIN, GPIO_PIN_RESET);
	  kb_gpio_set(LED4_PORT, LED4_PIN, GPIO_PIN_RESET);

}
