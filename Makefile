CC=gcc
CXX=g++
CFLAGS=-Wall
CXXFLAGS=-Wall
LDFLAGS=-lsdl2 -lglfw -lglew -lavformat -lavcodec -lswscale -framework opengl
BIN_DIR=bin

# List of source files
SOURCES_C=$(wildcard src/*.c)
SOURCES_CPP=$(wildcard src/*.cpp)

# Object files for the corresponding source files
OBJECTS_C=$(SOURCES_C:.c=.o)
OBJECTS_CPP=$(SOURCES_CPP:.cpp=.o)

# Target binaries from source files
TARGETS_C=$(patsubst src/%.c, $(BIN_DIR)/%, $(SOURCES_C))
TARGETS_CPP=$(patsubst src/%.cpp, $(BIN_DIR)/%, $(SOURCES_CPP))

# Default target
all: $(BIN_DIR) $(TARGETS_C) $(TARGETS_CPP)

# Rule to create the binary directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Rule to compile C source files
$(BIN_DIR)/%: src/%.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Rule to compile C++ source files
$(BIN_DIR)/%: src/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

# Rule to clean the workspace
clean:
	rm -rf $(BIN_DIR) $(OBJECTS_C) $(OBJECTS_CPP)

.PHONY: all clean

