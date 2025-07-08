# Default output name
EXE ?= weak
ifeq ($(OS),Windows_NT)
	EXE := $(EXE).exe
endif

# Required GCC version
GCC_REQUIRED_VERSION := 13

# Try to get installed GCC major version (fallback to 0 if failed)
GCC_VERSION := $(shell g++ -dumpversion | cut -d. -f1 2>/dev/null || echo 0)

# Use newer GCC if necessary
ifeq ($(shell [ $(GCC_VERSION) -lt $(GCC_REQUIRED_VERSION) ] && echo yes),yes)
	CXX := g++-$(GCC_REQUIRED_VERSION)
else
	CXX := g++
endif

# Compiler flags
CXXFLAGS := -std=c++17 -O3 -march=native

# Source files
SOURCES := $(wildcard *.cpp)

# Default build target
all: check-gcc
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXE)

# Check GCC version and install if necessary
check-gcc:
	@if [ $(GCC_VERSION) -lt $(GCC_REQUIRED_VERSION) ]; then \
		echo "GCC version is $(GCC_VERSION), installing GCC $(GCC_REQUIRED_VERSION)..."; \
		if command -v apt >/dev/null 2>&1; then \
			sudo apt update && sudo apt install -y g++-$(GCC_REQUIRED_VERSION); \
		elif command -v dnf >/dev/null 2>&1; then \
			sudo dnf install -y gcc-g++; \
		elif command -v brew >/dev/null 2>&1; then \
			brew install gcc; \
		else \
			echo "Unsupported package manager. Please install GCC $(GCC_REQUIRED_VERSION)+ manually."; \
			exit 1; \
		fi; \
	else \
		echo "GCC version $(GCC_VERSION) is sufficient."; \
	fi

# Clean build artifacts
clean:
	rm -f *.o *.exe Engine-* weak
