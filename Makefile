CC          := clang
CXX         := clang++
CFLAGS      := -Wall -Wextra $(shell pkg-config --cflags --static glew glfw3 sdl2 libavcodec libavformat libavutil libswresample libswscale)
CXXFLAGS    := $(CFLAGS)
LDFLAGS     := $(shell pkg-config --libs --static glew glfw3 sdl2 libavcodec libavformat libavutil libswresample libswscale) -framework OpenGL
BIN_DIR     := bin
SRC_DIR     := src
SOURCES_C   := $(wildcard $(SRC_DIR)/*.c)
SOURCES_CPP := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS_C   := $(SOURCES_C:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)
OBJECTS_CPP := $(SOURCES_CPP:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)
TARGETS     := $(OBJECTS_C:%.o=%) $(OBJECTS_CPP:%.o=%)

.PHONY: all clean

all: $(BIN_DIR) $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: $(BIN_DIR)/%.o
	$(CXX) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)
