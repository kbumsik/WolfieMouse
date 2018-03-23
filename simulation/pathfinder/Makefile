#######################################
# Project build settings
#######################################
# Project name
TARGET := pathfinder

# Build path
BUILD_DIR := build

# debug build?
DEBUG := 1
# optimization
OPTIMIZATION_FLAG = -Og
# Verbose gcc messages?
VERBOSE := 0
# GCC flags
CFLAGS = -Wall
CXXFLAGS = -Wall

# Compiler
CC := gcc
CXX := gcc

# Debugger
DEBUGGER := cdtdebug

#######################################
# Defines
#######################################
# macros for gcc
# C defines
C_DEFS =  \

#######################################
# Source files
#######################################

SOLVER_DIR = ../../firmware/src/maze

# .c files used in /firmware/src/maze
SOLVER_FILES = \
StdIO.cc \
MazeIO.cc \
PositionController.cc \
Maze.cc \
MouseController.cc \
SimulMouse.cc \
# fakeIO.cc \


######################################
# source
######################################

# C sources
C_SOURCES = $(wildcard ./*.c)
C_SOURCES += $(wildcard ./*.cc)

# maze sources
SOLVER_SOURCES := $(addprefix $(SOLVER_DIR)/, $(SOLVER_FILES))

C_SOURCES += $(SOLVER_SOURCES)

# C includes
C_INCLUDES =  \
-I$(SOLVER_DIR) \

ifeq ($(DEBUG), 1)
  CFLAGS += -g -ggdb
endif

CFLAGS += $(C_DEFS) $(C_INCLUDES) $(OPTIMIZATION_FLAG) -std=c99 -g -ggdb
CXXFLAGS += $(C_DEFS) $(C_INCLUDES) $(OPTIMIZATION_FLAG) -std=c++11 -g -ggdb
LDFLAGS := -lstdc++ -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(TARGET)

#######################################
# build the application
#######################################
# Verbose message
ifeq ($(VERBOSE), 1)
  CFLAGS += -v
  LDFLAGS += --verbose
endif

# Divide C and CPP
CXX_SOURCES := $(filter-out %.c,$(C_SOURCES))
C_SOURCES := $(filter-out %.cc,$(C_SOURCES))

# list of C objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))

# list of CPP objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CXX_SOURCES:.cc=.o)))

ABSPATH := $(shell pwd)
C_SOURCES := $(addprefix $(ABSPATH)/,$(C_SOURCES))
CXX_SOURCES := $(addprefix $(ABSPATH)/,$(CXX_SOURCES))
OBJECTS := $(addprefix $(ABSPATH)/,$(OBJECTS))
BUILD_DIR := $(addprefix $(ABSPATH)/,$(BUILD_DIR))


vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.cc $(sort $(dir $(CXX_SOURCES)))

# build C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -o $@


# build CPP files
$(BUILD_DIR)/%.o: %.cc | $(BUILD_DIR) 
	$(CC) -c $(CXXFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo ""
	@echo "===== Build Completed ====="
	@echo ""

$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
.PHONY: clean
clean:
	-rm -fR $(BUILD_DIR) $(TARGET)

#######################################
# Debugger
#######################################
.PHONY: debug
debug:
	$(DEBUGGER) -e $(TARGET)
