#######################################
# Project build settings
#######################################
# Project name
TARGET = blinky_nucleo64

MAZE_QUESTION = maze_ieee_region_1_2015.txt
MAZE_STRING_HPP = mazeString.hpp

# debug build?
DEBUG = 1
# optimization
OPTIMIZATION_FLAG = -Og
# Verbose gcc messages?
VERBOSE = 0

#######################################
# Hardware
#######################################
# board: Should be one of /firmware/src/board
BOARD = nucleo_f446re

# One of GCC flags
CPU_FLAG = -mcpu=cortex-m4
FPU_FLAG = -mfpu=fpv4-sp-d16
FLOAT_ABI_FLAG = -mfloat-abi=hard

#######################################
# CFLAGS
#######################################
# macros for gcc
# C defines
C_DEFS =  \
-DMAZE_QUESTION=\"$(MAZE_QUESTION)\" \
-DUSE_HAL_DRIVER \
-DSTM32F446xx \
-DOS_USE_TRACE_SEMIHOSTING_DEBUG \
-DHSE_VALUE=8000000

# AS defines
#AS_DEFS = 

# extra gcc flags
#ASFLAGS =
#CFLAGS =
#LDFLAGS =

#######################################
# Source files
#######################################
# .c files used in /firmware/lib/STM32F4xx_HAL_Driver/Src
STM32_HAL_FILES = \
stm32f4xx_hal.c \
stm32f4xx_hal_adc_ex.c \
stm32f4xx_hal_tim.c \
stm32f4xx_hal_rcc_ex.c \
stm32f4xx_hal_gpio.c \
stm32f4xx_hal_spi.c \
stm32f4xx_hal_flash_ramfunc.c \
stm32f4xx_hal_dma_ex.c \
stm32f4xx_hal_pwr.c \
stm32f4xx_hal_adc.c \
stm32f4xx_hal_i2c.c \
stm32f4xx_hal_cortex.c \
stm32f4xx_hal_uart.c \
stm32f4xx_hal_flash_ex.c \
stm32f4xx_hal_rcc.c \
stm32f4xx_hal_flash.c \
stm32f4xx_hal_i2c_ex.c \
stm32f4xx_hal_tim_ex.c \
stm32f4xx_hal_pwr_ex.c \
stm32f4xx_hal_dma.c \

# .c files used in /firmware/src/math
MATH_FILES = \
# pid.c

# .c files used in /firmware/src/maze
MAZE_FILES = \
FlashIO.cc \
FakeIO.cc \
Maze.cc \
MouseController.cc \
SimulMouse.cc \
MazeIO.cc \
PositionController.cc \
StdIO.cc \

# .c files used in /firmware/src/module
MODULE_FILES = \
terminal.c \
# hcms-290x_display.c \
# terminal_test.c \

# .c files used in /firmware/src/peripheral
PERIPHERAL_FILES = \
flash.c \
adc.c \
rotary_encoder.c \
gpio.c \
i2c.c \
spi.c \
tim.c \
uart.c \

#######################################
# Extra
#######################################
# link script.
#LDSCRIPT = STM32F446RETx_FLASH.ld
# Startup ASM code.
#STARTUP_ASM = startup_stm32f446xx.s
# FreeRTOS heap.
#FREERTOS_HEAP_FILE ?= heap_4.c


# These shoud be included to make it work
include ../source.mk
include ../toolchain.mk
include ../target.mk
include ../openocd.mk  # For flash, debug and gdb targets
