run: main
	./main
	
main: main.c
	cc -O3 main.c -o main -framework opengl -framework cocoa -framework iokit -lglfw
