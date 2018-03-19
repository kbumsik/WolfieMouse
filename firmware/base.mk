#######################################
# Base paths
#######################################
SRC_DIR = ../src
LIB_DIR = ../lib
BOARD_DIR = $(SRC_DIR)/board/$(BOARD)
BOARD_SRC_DIR = $(BOARD_DIR)
MATH_DIR = $(SRC_DIR)/math
MATH_SRC_DIR = $(MATH_DIR)
MAZE_DIR = $(SRC_DIR)/maze
MAZE_SRC_DIR = $(MAZE_DIR)
MODULE_DIR = $(SRC_DIR)/module
MODULE_SRC_DIR = $(MODULE_DIR)
PERIPHERAL_DIR = $(SRC_DIR)/peripheral
PERIPHERAL_SRC_DIR = $(PERIPHERAL_DIR)
SYSTEM_DIR = $(SRC_DIR)/system
SYSTEM_SRC_DIR = $(SYSTEM_DIR)

# Libs dirs
STM32_HAL_DIR = $(LIB_DIR)/STM32F4xx_HAL_Driver
STM32_HAL_SRC_DIR = $(STM32_HAL_DIR)/Src
FREERTOS_DIR = $(LIB_DIR)/FreeRTOS/Source
FREERTOS_SRC_DIR = $(FREERTOS_DIR)

NEWLIB_DIR = $(SYSTEM_DIR)/newlib
NEWLIB_SRC_DIR = $(NEWLIB_DIR)


#######################################
# paths
#######################################
# source path
SOURCES_DIR =  \
$(LIB_DIR)/STM32F4xx_HAL_Driver \
$(LIB_DIR)/FreeRTOS \
$(LIB_DIR)/CMSIS \
$(BOARD_DIR) \
$(MATH_DIR) \
$(MAZE_DIR) \
$(MODULE_DIR) \
$(PERIPHERAL_DIR) \
$(SYSTEM_DIR)

# Build path
BUILD_DIR := build

######################################
# source
######################################

# C sources
C_SOURCES = $(wildcard ./*.c)
C_SOURCES += $(wildcard ./*.cc)

# board sources
BOARD_SOURCES := $(addsuffix /*.c,$(BOARD_SRC_DIR))
BOARD_SOURCES := $(wildcard $(BOARD_SOURCES))

C_SOURCES += $(BOARD_SOURCES)

# math sources
MATH_SOURCES := $(addprefix $(MATH_SRC_DIR)/, $(MATH_FILES))

C_SOURCES += $(MATH_SOURCES)

# maze sources
MAZE_SOURCES := $(addprefix $(MAZE_SRC_DIR)/, $(MAZE_FILES))

C_SOURCES += $(MAZE_SOURCES)

# module sources
MODULE_SOURCES := $(addprefix $(MODULE_SRC_DIR)/, $(MODULE_FILES))

MODULE_EXCLUDE_FILES := terminal_test.c
MODULE_EXCLUDE_FILES := $(addprefix $(MODULE_SRC_DIR)/, $(MODULE_EXCLUDE_FILES))
MODULE_SOURCES := $(filter-out $(MODULE_EXCLUDE_FILES),$(MODULE_SOURCES))

C_SOURCES += $(MODULE_SOURCES)

# peripheral sources
PERIPHERAL_SOURCES := $(addprefix $(PERIPHERAL_SRC_DIR)/, $(PERIPHERAL_FILES))

C_SOURCES += $(PERIPHERAL_SOURCES)

# system sources
SYSTEM_SOURCES := $(addsuffix /*.c,$(SYSTEM_SRC_DIR))
SYSTEM_SOURCES := $(wildcard $(SYSTEM_SOURCES))

SYSTEM_EXCLUDE_FILES := system_config_tmplate.c
SYSTEM_EXCLUDE_FILES := $(addprefix $(SYSTEM_SRC_DIR)/, $(SYSTEM_EXCLUDE_FILES))
SYSTEM_SOURCES := $(filter-out $(SYSTEM_EXCLUDE_FILES),$(SYSTEM_SOURCES))

C_SOURCES += $(SYSTEM_SOURCES)

# STM32 HAL sources
ifndef STM32_HAL_FILES
  $(error STM32_HAL_FILES is undefined)
endif
STM32_HAL_SOURCES := $(addprefix $(STM32_HAL_SRC_DIR)/, $(STM32_HAL_FILES))

C_SOURCES += $(STM32_HAL_SOURCES)

# FreeRTOS sources
FREERTOS_HEAP_FILE ?= heap_4.c
FREERTOS_SOURCES := \
$(FREERTOS_SRC_DIR)/portable/MemMang/$(FREERTOS_HEAP_FILE) \
$(FREERTOS_SRC_DIR)/event_groups.c \
$(FREERTOS_SRC_DIR)/queue.c \
$(FREERTOS_SRC_DIR)/list.c \
$(FREERTOS_SRC_DIR)/tasks.c \
$(FREERTOS_SRC_DIR)/croutine.c \
$(FREERTOS_SRC_DIR)/timers.c \
$(FREERTOS_SRC_DIR)/portable/GCC/ARM_CM4F/port.c \
$(FREERTOS_SRC_DIR)/CMSIS_RTOS/cmsis_os.c

C_SOURCES += $(FREERTOS_SOURCES)

# newlib implementation sources
NEWLIB_SOURCES := $(addsuffix /*.c,$(NEWLIB_SRC_DIR))
NEWLIB_SOURCES := $(wildcard $(NEWLIB_SOURCES))

C_SOURCES += $(NEWLIB_SOURCES)


# ASM sources
STARTUP_ASM ?= $(BOARD_DIR)/startup.s
ASM_SOURCES =  \
$(STARTUP_ASM)

#######################################
# binaries
#######################################
PREFIX := arm-none-eabi-
CC := $(PREFIX)gcc
AS := $(PREFIX)gcc -x assembler-with-cpp
CP := $(PREFIX)objcopy
AR := $(PREFIX)ar
SZ := $(PREFIX)size
HEX := $(CP) -O ihex
BIN := $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# mcu_flags
MCU_FLAGS := $(CPU_FLAG) -mthumb $(FPU_FLAG) $(FLOAT_ABI_FLAG)

# macros for gcc
# AS defines
AS_DEFS += 

# C defines
C_DEFS +=

# AS includes
AS_INCLUDES =  \

# C includes
C_INCLUDES =  \
-I$(LIB_DIR)/CMSIS/Include \
-I$(LIB_DIR)/CMSIS/Device/ST/STM32F4xx/Include \
-I$(STM32_HAL_DIR)/Inc \
-I$(STM32_HAL_DIR)/Inc/Legacy \
-I$(FREERTOS_DIR)/portable/GCC/ARM_CM4F \
-I$(FREERTOS_DIR)/include \
-I$(FREERTOS_DIR)/CMSIS_RTOS \
-I$(BOARD_SRC_DIR) \
-I$(MATH_SRC_DIR) \
-I$(MAZE_SRC_DIR) \
-I$(MODULE_SRC_DIR) \
-I$(PERIPHERAL_SRC_DIR) \
-I$(SYSTEM_SRC_DIR) \

# compile gcc flags
ASFLAGS += $(MCU_FLAGS) $(AS_DEFS) $(AS_INCLUDES) $(OPTIMIZATION_FLAG) \
			-Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU_FLAGS) $(C_DEFS) $(C_INCLUDES) $(OPTIMIZATION_FLAG) \
			-Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
  CFLAGS += -g -ggdb
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT ?= $(BOARD_DIR)/linker_script.ld

# libraries
LIB_FLAGS := -lc -lm -lnosys
LDFLAGS += $(MCU_FLAGS) -specs=nano.specs -T$(LDSCRIPT) $(LIB_FLAGS) \
			-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# Verbose message
ifeq ($(VERBOSE), 1)
  CFLAGS += -v
  LDFLAGS += --verbose
endif

# Divide C and CPP
CPP_SOURCES := $(filter-out %.c,$(C_SOURCES))
C_SOURCES := $(filter-out %.cc,$(C_SOURCES))

# list of C objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# build C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -std=gnu99 -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# list of CPP objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cc=.o)))
vpath %.cc $(sort $(dir $(CPP_SOURCES)))

# build CPP files
$(BUILD_DIR)/%.o: %.cc | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -std=c++11 -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cc=.lst)) $< -o $@

# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# build ASM files
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo ""
	@echo "===== Build Completed ====="
	$(SZ) $@
	@echo ""

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
.PHONY: clean
clean:
	-rm -fR $(BUILD_DIR)
