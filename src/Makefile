# Default output executable name (auto .exe on Windows)
EXE ?= weak
ifeq ($(OS),Windows_NT)
	EXE := $(EXE).exe
endif

# Compiler and flags
CXX := g++
CXXFLAGS := -O3

# Source files
SOURCES := $(wildcard *.cpp)

# Build target
all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXE)

# Clean target
clean:
	rm -f *.o *.exe Engine-* weak
