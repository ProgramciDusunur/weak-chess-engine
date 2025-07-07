# Default output executable name
EXE ?= weak.exe

# Compiler and flags
CXX := g++
CXXFLAGS := -O3

# Source files (all .cpp in current directory)
SOURCES := $(wildcard *.cpp)

# Build target
all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXE)

# Clean target
clean:
	rm -f *.o *.exe Engine-* weak.exe
