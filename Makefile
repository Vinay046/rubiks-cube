# Compiler
CXX := g++

# Compiler flags
CXXFLAGS := -std=c++11 -Wextra

# Executable name
EXECUTABLE := main

# Directories
SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Include paths
INCLUDES := -I$(INC_DIR)

# Libraries for linking (OpenGL and related)
LIBS := -lGL -lGLU -lglfw -lglut -lGLEW

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJS)

# Create the directories used in the build
dirs:
	@mkdir -p $(OBJ_DIR) $(SRC_DIR) $(INC_DIR)

.PHONY: all clean dirs
