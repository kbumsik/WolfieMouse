#######################################
# Project build settings
#######################################
# Project name
TARGET = blinky_nucleo64

# debug build?
DEBUG = 1
# optimization
OPTIMIZATION_FLAG = -Og

#######################################
# Hardware
#######################################
# board: Should be one of /firmware/src/baord
BOARD = nucleo_f446re

# One of GCC flags
CPU_FLAG = -mcpu=cortex-m4
FPU_FLAG = -mfpu=fpv4-sp-d16
FLOAT_ABI_FLAG = -mfloat-abi=hard

#######################################
# CFLAGS
#######################################
# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F446xx

# extra gcc flags
ASFLAGS =
CFLAGS =
LDFLAGS =

#######################################
# Extra
#######################################
# link script. Uncomment it if you have your own linker script.
# LDSCRIPT = STM32F446RETx_FLASH.ld
# STARTUP_ASM = startup_stm32f446xx.s

# These shoud be included to make it work
include ../base.mk
include ../openocd.mk  # For flash, debug and gdb targets
