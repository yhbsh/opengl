<<<<<<< HEAD
main: main.c
	cc -O3 main.c -o main -framework opengl -framework cocoa -framework iokit -lglfw3
=======
run: main
	./main
main: main.c
	cc main.c -o main -framework opengl -lglfw
>>>>>>> 4ae286006ab8ab3647e3beb73df44417a1e7784b
