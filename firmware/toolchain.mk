#######################################
# Detect Environment
#######################################
OS := $(shell uname)

# Linux specific
ifeq ($(OS),Linux)
  # Detect Windows Subsystem for Linux
  MS_WSL := $(shell if grep -q Microsoft /proc/version; then echo Microsoft; fi)
  ifeq ($(MS_WSL),Microsoft)
    OS := WSL
  endif
endif

# macOS specific
ifeq ($(OS),Darwin)
endif

#######################################
# Toolchain
#######################################
PREFIX := arm-none-eabi-
ifeq ($(OS),WSL)
  SUFFIX := .exe
endif
ifeq ($(OS),Linux)
  SUFFIX := ""
endif
ifeq ($(OS),Darwin)
  SUFFIX := ""
endif

CC := $(PREFIX)gcc$(SUFFIX) -std=gnu99 -fdiagnostics-color=auto
CXX := $(PREFIX)g++$(SUFFIX) -std=c++11 -fdiagnostics-color=auto
AS := $(PREFIX)gcc$(SUFFIX) -fdiagnostics-color=auto -x assembler-with-cpp
CP := $(PREFIX)objcopy$(SUFFIX)
AR := $(PREFIX)ar$(SUFFIX)
SZ := $(PREFIX)size$(SUFFIX)
HEX := $(CP) -O ihex
BIN := $(CP) -O binary -S
OPENOCD := openocd$(SUFFIX)

#######################################
# Flags
#######################################
# MCU flags
CPU_FLAG ?= -mcpu=cortex-m4
FPU_FLAG ?= -mfpu=fpv4-sp-d16
FLOAT_ABI_FLAG ?= -mfloat-abi=hard
MCU_FLAGS := $(CPU_FLAG) -mthumb $(FPU_FLAG) $(FLOAT_ABI_FLAG)

# optimization
OPTIMIZATION_FLAG ?= -Og

# macros for gcc
# AS defines
AS_DEFS += 

# C defines
C_DEFS +=

# gcc flags
ASFLAGS += $(MCU_FLAGS) $(AS_DEFS) $(AS_INCLUDES) $(OPTIMIZATION_FLAG) \
			-Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU_FLAGS) $(C_DEFS) $(C_INCLUDES) $(OPTIMIZATION_FLAG) \
			-Wall -fdata-sections -ffunction-sections

# linker flags and libraries
LIB_FLAGS := -lc -lm -lnosys -lstdc++
LDFLAGS += $(MCU_FLAGS) -specs=nano.specs -T$(LDSCRIPT) $(LIB_FLAGS) \
			-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

ifeq ($(DEBUG), 1)
  CFLAGS += -g -ggdb
endif

# Verbose message
ifeq ($(VERBOSE), 1)
  CFLAGS += -v
  LDFLAGS += --verbose
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"

# C/C++ specific flags
CXXFLAGS = $(CFLAGS)
