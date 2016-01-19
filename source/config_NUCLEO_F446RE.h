/*
 * config_NUCLEO_F411RE.h
 *
 *  Created on: Dec 21, 2015
 *      Author: Bumsik Kim
 */

#ifndef CONFIG_NUCLEO_F411RE_H_
#define CONFIG_NUCLEO_F411RE_H_

/**
 * @brief defines for CMSIS and HAL
 */

#ifndef STM32F446xE
    #define STM32F446xE
#endif

#ifndef ARM_MATH_CM4
    #define ARM_MATH_CM4
#endif


#ifndef USE_HAL_DRIVER
    #define USE_HAL_DRIVER
#endif

/**
 * @defgroup UART settings
 * @brief Pin and Peripheral settings for UART
 * @{
 */
#ifdef confNUCLEO_USE_STLINK_COM_PORT_FOR_UART
  /* Peripheral number definition */
  #define uartUARTx     USART2
  /* Definition for USARTx clock resources */
  #define uartCLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE();
  #define uartRX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
  #define uartTX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
  #define uartFORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
  #define uartRELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()
  
  /* Definition for USARTx Pins */
  #define uartTX_PIN                    GPIO_PIN_2
  #define uartTX_GPIO_PORT              GPIOA  
  #define uartTX_AF                     GPIO_AF7_USART2
  #define uartRX_PIN                    GPIO_PIN_3
  #define uartRX_GPIO_PORT              GPIOA 
  #define uartRX_AF                     GPIO_AF7_USART2

  /* Definition for UART Initialization */
  #define uartBAUD_RATE   115200
  #define uartWORD_LENGTH UART_WORDLENGTH_8B
  #define uartSTOP_BITS   UART_STOPBITS_1
  #define uartPARITY     UART_PARITY_NONE

  /* Definition for UART Interrupt */
  #define uartUSARTx_IRQn               USART2_IRQn
  #define uartUSARTx_IRQHandler         USART2_IRQHandler
#else
  /* Peripheral number definition */
  #define uartUARTx     USART1
  /* Definition for USARTx clock resources */
  #define uartCLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
  #define uartRX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
  #define uartTX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
  #define uartFORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
  #define uartRELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()
  
  /* Definition for USARTx Pins */
  #define uartTX_PIN                    GPIO_PIN_15
  #define uartTX_GPIO_PORT              GPIOA  
  #define uartTX_AF                     GPIO_AF7_USART1
  #define uartRX_PIN                    GPIO_PIN_10
  #define uartRX_GPIO_PORT              GPIOA 
  #define uartRX_AF                     GPIO_AF7_USART1

  /* Definition for UART Initialization */
  #define uartBAUD_RATE   115200
  #define uartWORD_LENGTH UART_WORDLENGTH_8B
  #define uartSTOP_BITS   UART_STOPBITS_1
  #define uartPARITY     UART_PARITY_NONE

  /* Definition for UART Interrupt */
  #define uartUSARTx_IRQn               USART1_IRQn
  #define uartUSARTx_IRQHandler         USART1_IRQHandler
#endif
/**
 * @}
 */

/**
 * @defgroup Motor settings
 * @brief Pin and Peripheral settings for Motor
 */
/* definition of motor */
#define MOTOR_TIMx                          TIM4
#define MOTOR_TIMx_CLK_ENABLE()             __HAL_RCC_TIM4_CLK_ENABLE()
#define MOTOR_TIMx_CLK_DISABLE()            __HAL_RCC_TIM4_CLK_DISABLE()
/* Definition for TIMx Channel Pins */
#define MOTOR_GPIO_PORT                     GPIOB
#define MOTOR_TIMx_CHANNEL_GPIO_PORT()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define MOTOR_GPIO_ALTERNATE                GPIO_AF2_TIM4
#define MOTOR_GPIO_PIN_CHANNEL_1            GPIO_PIN_6
#define MOTOR_GPIO_PIN_CHANNEL_2            GPIO_PIN_7
#define MOTOR_GPIO_PIN_CHANNEL_3            GPIO_PIN_8
#define MOTOR_GPIO_PIN_CHANNEL_4            GPIO_PIN_9


/**
 * @defgroup NRF24L01+ Settings
 * @brief    Pin settings for NRF24L01+
 * @verbatim
NRF24L01+   STM32Fxxx   DESCRIPTION

GND         GND         Ground
VCC         3.3V        3.3V
CE          PC2     RF activated pin
CSN         PC3     Chip select pin for SPI
SCK         PC10        SCK pin for SPI
MOSI        PC12        MOSI pin for SPI
MISO        PC11        MISO pin for SPI
IRQ         Not used(PD2)   Interrupt pin. Goes low when active. Pin functionality is active, but not used in library
@endverbatim
 * @{
 */
/* Default SPI used */
#define NRF24L01_SPI                SPI3
#define NRF24L01_SPI_PINS           TM_SPI_PinsPack_2

/* SPI chip enable pin */
#define NRF24L01_CSN_PORT               GPIOC
#define NRF24L01_CSN_PIN                GPIO_PIN_2
#define NRF24L01_CSN_PORT_CLK_ENABLE    __HAL_RCC_GPIOC_CLK_ENABLE

/* Chip enable for transmitting */
#define NRF24L01_CE_PORT                GPIOC
#define NRF24L01_CE_PIN                 GPIO_PIN_3
#define NRF24L01_CE_PORT_CLK_ENABLE     __HAL_RCC_GPIOC_CLK_ENABLE

/**
 * @}
 */


/**
  @page TIM_PWMInput TIM PWM Input example

@par Example Description

This example shows how to use the TIM peripheral to measure the frequency and
duty cycle of an external signal.

The TIM4CLK frequency is set to SystemCoreClock (Hz), the Prescaler is 0 so the
counter clock is SystemCoreClock (Hz).
The SystemCoreClock is set to 100 MHz for STM32F411xEx Devices.

TIM4 is configured in PWM Input Mode: the external signal is connected to
TIM4 Channel2 used as input pin.
To measure the frequency and the duty cycle we use the TIM4 CC2 interrupt request,
so In the TIM4_IRQHandler routine, the frequency and the duty cycle of the external
signal are computed.
The "uwFrequency" variable contains the external signal frequency:
TIM4 counter clock = SystemCoreClock,
Frequency = TIM4 counter clock / TIM4_CCR2 in Hz,
The "uwDutyCycle" variable contains the external signal duty cycle:
DutyCycle = (TIM4_CCR1*100)/(TIM4_CCR2) in %.

The minimum frequency value to measure is (TIM4 counter clock / CCR MAX)
                                         = (100 MHz)/ 65535
                                         = 1526 Hz

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note The clock setting is configured to have the max product performance (max clock frequency)
      so not optimized in term of power consumption.

@par Directory contents

  - TIM/TIM_PWMInput/Inc/stm32f4xx_hal_conf.h    HAL configuration file
  - TIM/TIM_PWMInput/Inc/stm32f4xx_it.h          Interrupt handlers header file
  - TIM/TIM_PWMInput/Inc/main.h                  Main program header file
  - TIM/TIM_PWMInput/Src/stm32f4xx_it.c          Interrupt handlers
  - TIM/TIM_PWMInput/Src/main.c                  Main program
  - TIM/TIM_PWMInput/Src/stm32f4xx_hal_msp.c     HAL MSP module
  - TIM/TIM_PWMInput/Src/system_stm32f4xx.c      STM32F4xx system clock configuration file


@par Hardware and Software environment

  - This example runs on STM32F411xEx devices.

  - This example has been tested with STMicroelectronics STM32F4xx-Nucleo RevC
    boards and can be easily tailored to any other supported device
    and development board.

  - STM32F4xx-Nucleo RevC Set-up
    - Connect the external signal to measure to the TIM4 CH2 pin PB.07 (pin 21 in CN7
      connector).


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 *
@endverbatim
 * @{
 */
/* User can use this section to tailor TIMx instance used and associated
   resources */
/* Definition for TIMx clock resources */
#define PWMinput_TIMx                           TIM3
#define PWMinput_TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()

/* Definition for PWMinput_TIMx Pins */
#define PWMinput_TIMx_CHANNEL_GPIO_PORT()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define GPIO_PORT                      GPIOB
#define GPIO_PIN_CHANNEL2              GPIO_PIN_5
#define GPIO_AF_PWMinput_TIMx                   GPIO_AF2_TIM3

/* Definition for PWMinput_TIMx's NVIC */
#define PWMinput_TIMx_IRQn                      TIM3_IRQn
#define PWMinput_TIMx_IRQHandler                TIM3_IRQHandler

/**
 * @}
 */

/**
 * @defgroup LED settings
 * @{
 */
#define portLED_0_PIN	GPIO_PIN_5
#define portLED_0_PORT	GPIOA

#ifdef USE_HAL_DRIVER
	#define vLED_0_Toggle()	HAL_GPIO_TogglePin(portLED_0_PORT, portLED_0_PIN)
	#define vLED_0_On()		HAL_GPIO_WritePin(portLED_0_PORT, portLED_0_PIN, GPIO_PIN_SET)
	#define vLED_0_Off()	HAL_GPIO_WritePin(portLED_0_PORT, portLED_0_PIN, GPIO_PIN_RESET)
#else
	#error "Not using HAL so no LED pins are defined!"
#endif
/**
 * @}
 */

#endif /* CONFIG_NUCLEO_F411RE_H_ */
