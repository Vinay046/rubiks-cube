# Compiler to use
CXX := g++
# Compiler flags
CXXFLAGS := -std=c++11 -Wextra

# Executable name
EXECUTABLE := main

# Source files
SRCS := main.cpp

# Libraries for linking (OpenGL and GLU)
LIBS := -lGL -lGLU -lglfw -lglut -lGLEW

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(EXECUTABLE) $(LIBS)

clean:
	rm -f $(EXECUTABLE)
