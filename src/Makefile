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
GCC_VERSION := $(shell $(CXX) -dumpfullversion | cut -d. -f1 2>/dev/null || echo 0)

# Check and upgrade GCC if needed
check-gcc:
	@echo "Checking GCC version..."
	@if [ $(GCC_VERSION) -lt 13 ]; then \
		echo "Detected GCC version $(GCC_VERSION). Installing latest G++..."; \
		if command -v apt >/dev/null 2>&1; then \
			sudo apt update && sudo apt install -y g++ gcc; \
		elif command -v dnf >/dev/null 2>&1; then \
			sudo dnf install -y gcc-c++; \
		elif command -v brew >/dev/null 2>&1; then \
			brew install gcc; \
		else \
			echo "Unsupported package manager. Please install a newer g++ manually."; \
			exit 1; \
		fi; \
		echo "Please restart your terminal or re-run make to use the updated compiler."; \
		exit 1; \
	else \
		echo "GCC version $(GCC_VERSION) is sufficient."; \
	fi

# Build target
all: check-gcc build

build:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXE)

# Clean target
clean:
	rm -f *.o *.exe Engine-* weak
