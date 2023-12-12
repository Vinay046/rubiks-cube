# Compiler to use
CXX := g++
# Compiler flags
CXXFLAGS := -std=c++11 -Wextra $(shell pkg-config --cflags opencv)

# Executable name
EXECUTABLE := main

# Source files
SRCS := main.cpp

OPENCV_LIBS := $(shell pkg-config --libs opencv)

# Libraries for linking (OpenGL and GLU)
LIBS := -lGL -lGLU -lglfw -lglut -lGLEW

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(EXECUTABLE) $(LIBS)

clean:
	rm -f $(EXECUTABLE)
