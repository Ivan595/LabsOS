all: my_chmod
my_chmod: main.o Chmod.o
	gcc main.o Chmod.o -o my_chmod
main.o: main.c
	gcc -c main.c
Chmod.o: Chmod.c
	gcc -c Chmod.c
clean:
	rm -rf *.o my_chmod
