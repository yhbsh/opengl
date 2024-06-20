CC=clang

LIBRARIES := glfw3 sdl2 libavcodec libavformat libavutil libswscale
FRAMEWORKS := -framework OpenGL
CFLAGS  := -Wall -Wextra -O3
CFLAGS  := $(shell pkg-config --cflags $(LIBRARIES)) $(CFLAGS)
LDFLAGS := $(shell pkg-config --libs   $(LIBRARIES)) $(FRAMEWORKS)
BIN_DIR := bin

SOURCES_C=$(wildcard src/*.c)
SOURCES_CPP=$(wildcard src/*.cpp)

OBJECTS_C=$(SOURCES_C:.c=.o)
OBJECTS_CPP=$(SOURCES_CPP:.cpp=.o)

TARGETS_C=$(patsubst src/%.c, $(BIN_DIR)/%, $(SOURCES_C))
TARGETS_CPP=$(patsubst src/%.cpp, $(BIN_DIR)/%, $(SOURCES_CPP))

all: $(BIN_DIR) $(TARGETS_C) $(TARGETS_CPP)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: src/%.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR) $(OBJECTS_C) $(OBJECTS_CPP)

.PHONY: all clean

