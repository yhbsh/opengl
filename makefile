run: main
	./main
main: main.c
	cc main.c -o main -framework opengl -lglfw
