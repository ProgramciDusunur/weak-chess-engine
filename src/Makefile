# Default output name
EXE ?= weak
ifeq ($(OS),Windows_NT)
	EXE := $(EXE).exe
endif

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -O3 -march=native

SOURCES := $(wildcard *.cpp)

# Detect current GCC version (major)
GCC_VERSION := $(shell $(CXX) -dumpversion | cut -d. -f1 2>/dev/null || echo 0)

# Install latest G++ if version is too old
check-gcc:
	@if [ $(GCC_VERSION) -lt 13 ]; then \
		echo "Detected GCC version $(GCC_VERSION). Installing the latest available G++..."; \
		if command -v apt >/dev/null 2>&1; then \
			sudo apt update && sudo apt install -y g++; \
		elif command -v dnf >/dev/null 2>&1; then \
			sudo dnf install -y gcc-g++; \
		elif command -v brew >/dev/null 2>&1; then \
			brew install gcc; \
		else \
			echo "Unsupported package manager. Please install a newer version of g++ manually."; \
			exit 1; \
		fi \
	else \
		echo "G++ version $(GCC_VERSION) is sufficient."; \
	fi

# Build target
all: check-gcc
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXE)

# Clean target
clean:
	rm -f *.o *.exe Engine-* weak
