test: main

all: main rectangle triangle ppm frame_buffer player texture

main: src/main.cpp
	c++ -O3 src/main.cpp -o bin/main -lglew -lglfw -framework opengl
	@./bin/main

rectangle: src/rectangle.c
	cc -O3 src/rectangle.c -o bin/rectangle -lglfw -lglew -framework opengl

triangle: src/triangle.c
	cc -O3 src/triangle.c -o bin/triangle -lglfw -lglew -framework opengl

ppm: src/ppm.c
	cc -O3 src/ppm.c -o bin/ppm -lglfw -lglew -framework opengl

frame_buffer: src/frame_buffer.c
	cc -O3 src/frame_buffer.c -o bin/frame_buffer -lsdl2

player: src/player.c
	cc -O3 src/player.c -o bin/player -lavformat -lavcodec -lavutil -lswscale -lglew -lglfw -framework opengl

texture: src/texture.cpp
	c++ -O3 src/texture.cpp -o bin/texture -lglew -lglfw -framework opengl