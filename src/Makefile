# Default base name for output binary
EXE ?= weak

# Append .exe on Windows only
ifeq ($(OS),Windows_NT)
	EXE := $(EXE).exe
endif

# Compiler and optimization flags
CXX := g++
CXXFLAGS := -O3

# All .cpp files in the current directory
SOURCES := $(wildcard *.cpp)

# Default build target
all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXE)

# Clean up binaries
clean:
	rm -f *.o *.exe Engine-* weak
