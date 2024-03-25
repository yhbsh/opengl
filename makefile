all: main rectangle triangle ppm frame_buffer player

main: main.c
	cc -O3 main.c -o main -lglew -lglfw -framework opengl

rectangle: rectangle.c
	cc -O3 rectangle.c -o rectangle -lglfw -lglew -framework opengl

triangle: triangle.c
	cc -O3 triangle.c -o triangle -lglfw -lglew -framework opengl

ppm: ppm.c
	cc -O3 ppm.c -o ppm -lglfw -lglew -framework opengl

frame_buffer: frame_buffer.c
	cc -O3 frame_buffer.c -o frame_buffer -lsdl2

player: player.c
	cc -O3 player.c -o player -lavformat -lavcodec -lavutil -lswscale -lglew -lglfw -framework opengl