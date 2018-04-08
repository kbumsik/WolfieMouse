#######################################
# Project build settings
#######################################
# Project name
TARGET = maze_solver

# Project-specific files
MAZE_QUESTION = maze_ieee_region_1_2015.txt
MAZE_SAVED	= out.txt
MAZE_OUT = out.txt

# macros for gcc
# C defines
C_DEFS =  \
-DMAZE_QUESTION=\"$(MAZE_QUESTION)\" \
-DMAZE_SAVED=\"$(MAZE_SAVED)\" \
-DMAZE_OUT=\"$(MAZE_OUT)\"

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

$(TARGET): out.txt mazeString.hpp $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo ""
	@echo "===== Build Completed ====="
	@echo ""
	@echo "Please run ./$(TARGET) to start the program"

out.txt:
	cp out_original.txt out.txt

mazeString.hpp: $(MAZE_QUESTION)
	python mazeTextToCCString.py $^ > $@

$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
.PHONY: clean
clean:
	-rm -fR $(BUILD_DIR) $(TARGET) mazeString.hpp
	cp out_original.txt out.txt

#######################################
# Debugger
#######################################
.PHONY: debug
debug:
	$(DEBUGGER) -e $(TARGET)
