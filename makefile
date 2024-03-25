all: main rectangle triangle ppm

main: main.c
	cc -O3 main.c -o main -lsdl2

rectangle: rectangle.c
	cc -O3 rectangle.c -o rectangle -lglfw -lglew -framework opengl

triangle: triangle.c
	cc -O3 triangle.c -o triangle -lglfw -lglew -framework opengl

ppm: ppm.c
	cc -O3 ppm.c -o ppm -lglfw -lglew -framework opengl
