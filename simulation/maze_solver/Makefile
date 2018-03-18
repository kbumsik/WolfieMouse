#######################################
# Project build settings
#######################################
# Project name
TARGET = maze_solver

# Build path
BUILD_DIR := build

# debug build?
DEBUG = 1
# optimization
OPTIMIZATION_FLAG = -Og
# Verbose gcc messages?
VERBOSE = 0
# GCC flags
CFLAGS = -Wall

# Compiler
CC := gcc

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
StdIO.cpp \
MazeIO.cpp \
PositionController.cpp \
Maze.cpp \
MouseController.cpp \
SimulMouse.cpp \
# fakeIO.cpp \


######################################
# source
######################################

# C sources
C_SOURCES = $(wildcard ./*.c)
C_SOURCES += $(wildcard ./*.cpp)

# maze sources
SOLVER_SOURCES := $(addprefix $(SOLVER_DIR)/, $(SOLVER_FILES))

C_SOURCES += $(SOLVER_SOURCES)

# C includes
C_INCLUDES =  \
-I$(SOLVER_DIR) \

ifeq ($(DEBUG), 1)
  CFLAGS += -g -gdwarf-2
endif

CFLAGS += $(C_DEFS) $(C_INCLUDES) $(OPTIMIZATION_FLAG) -Wall
LDFLAGS := -lstdc++

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
CPP_SOURCES := $(filter-out %.c,$(C_SOURCES))
C_SOURCES := $(filter-out %.cpp,$(C_SOURCES))

# list of C objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# build C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -o $@

# list of CPP objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

# build CPP files
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) $< -o $@

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
	-rm -fR $(BUILD_DIR)
