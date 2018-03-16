#######################################
# Base paths
#######################################
SRC_DIR = ../src
LIB_DIR = ../lib
BOARD_DIR = $(SRC_DIR)/board/$(BOARD)

# Libs dirs
STM32_HAL_DIR = $(LIB_DIR)/STM32F4xx_HAL_Driver
STM32_HAL_SRC_DIR = $(STM32_HAL_DIR)/Src
FREERTOS_DIR = $(LIB_DIR)/FreeRTOS/Source
FREERTOS_SRC_DIR = $(FREERTOS_DIR)

#######################################
# paths
#######################################
# source path
SOURCES_DIR =  \
$(LIB_DIR)/STM32F4xx_HAL_Driver \
$(LIB_DIR)/FreeRTOS \
$(LIB_DIR)/CMSIS \
$(SRC_DIR)/math \
$(SRC_DIR)/maze \
$(SRC_DIR)/module \
$(SRC_DIR)/peripheral \
$(SRC_DIR)/system \
$(BOARD_DIR)

# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_adc_ex.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_tim.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_rcc_ex.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_gpio.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_spi.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_flash_ramfunc.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_dma_ex.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_pwr.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_adc.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_i2c.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_cortex.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_uart.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_flash_ex.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_rcc.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_flash.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_i2c_ex.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_tim_ex.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_pwr_ex.c \
$(STM32_HAL_SRC_DIR)/stm32f4xx_hal_dma.c \
$(FREERTOS_SRC_DIR)/portable/MemMang/heap_4.c \
$(FREERTOS_SRC_DIR)/event_groups.c \
$(FREERTOS_SRC_DIR)/queue.c \
$(FREERTOS_SRC_DIR)/list.c \
$(FREERTOS_SRC_DIR)/tasks.c \
$(FREERTOS_SRC_DIR)/croutine.c \
$(FREERTOS_SRC_DIR)/timers.c \
$(FREERTOS_SRC_DIR)/portable/GCC/ARM_CM4F/port.c \
$(FREERTOS_SRC_DIR)/CMSIS_RTOS/cmsis_os.c \
$(SRC_DIR)/src/system_stm32f4xx.c \
$(SRC_DIR)/src/main.c \
$(SRC_DIR)/src/freertos.c \
$(SRC_DIR)/src/stm32f4xx_it.c \
$(SRC_DIR)/src/stm32f4xx_hal_msp.c 

# ASM sources
STARTUP_ASM = $(BOARD_DIR)/startup.s
ASM_SOURCES =  \
$(STARTUP_ASM)

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
AR = $(PREFIX)ar
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################

# mcu_flags
MCU_FLAGS = $(CPU_FLAG) -mthumb $(FPU_FLAG) $(FLOAT_ABI_FLAG)

# macros for gcc
# AS defines
AS_DEFS += 

# C defines
C_DEFS +=

# AS includes
AS_INCLUDES =  \
-I$(SRC_DIR)/inc

# C includes
C_INCLUDES =  \
-I$(SRC_DIR)/inc \
-I$(LIB_DIR)/CMSIS/Device/ST/STM32F4xx/Include \
-I$(STM32_HAL_DIR)/Inc \
-I$(STM32_HAL_DIR)/Inc/Legacy \
-I$(FREERTOS_DIR)/portable/GCC/ARM_CM4F \
-I$(FREERTOS_DIR)/include \
-I$(FREERTOS_DIR)/CMSIS_RTOS \
-I$(LIB_DIR)/CMSIS/Include

# compile gcc flags
ASFLAGS += $(MCU_FLAGS) $(AS_DEFS) $(AS_INCLUDES) $(OPTIMIZATION_FLAG) \
			-Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU_FLAGS) $(C_DEFS) $(C_INCLUDES) $(OPTIMIZATION_FLAG) \
			-Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = $(BOARD_DIR)/linker_script.ld

# libraries
LIB_FLAGS = -lc -lm -lnosys
LDFLAGS += $(MCU_FLAGS) -specs=nano.specs -T$(LDSCRIPT) $(LIB_FLAGS) \
			-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# build C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# build ASM files
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
