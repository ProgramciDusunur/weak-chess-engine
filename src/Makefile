# Default output name
EXE ?= weak
ifeq ($(OS),Windows_NT)
	EXE := $(EXE).exe
endif

# Target GCC version
GCC_REQUIRED_VERSION := 13

# Determine installed GCC version
GCC_VERSION := $(shell g++ -dumpversion | cut -d. -f1)

# Choose compiler conditionally
ifeq ($(shell [ $(GCC_VERSION) -lt $(GCC_REQUIRED_VERSION) ] && echo yes),yes)
	CXX := g++-$(GCC_REQUIRED_VERSION)
else
	CXX := g++
endif

CXXFLAGS := -O3 -march=native
SOURCES := $(wildcard *.cpp)

all: check-gcc
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXE)

check-gcc:
	@if [ $(GCC_VERSION) -lt $(GCC_REQUIRED_VERSION) ]; then \
		echo "GCC version is $(GCC_VERSION), installing GCC $(GCC_REQUIRED_VERSION)..."; \
		if [ -x "$$(command -v apt)" ]; then \
			sudo apt update && sudo apt install -y g++-$(GCC_REQUIRED_VERSION); \
		elif [ -x "$$(command -v dnf)" ]; then \
			sudo dnf install -y gcc-g++; \
		elif [ -x "$$(command -v brew)" ]; then \
			brew install gcc; \
		else \
			echo "Package manager not supported. Please install GCC $(GCC_REQUIRED_VERSION)+ manually."; \
			exit 1; \
		fi; \
	else \
		echo "GCC version $(GCC_VERSION) is sufficient."; \
	fi

clean:
	rm -f *.o *.exe Engine-* weak
