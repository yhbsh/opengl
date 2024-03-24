all: main triangle

main: main.c
	cc -O3 main.c -o main -lglfw -lglew -framework opengl

triangle: triangle.c
	cc -O3 triangle.c -o triangle -lglfw -lglew -framework opengl