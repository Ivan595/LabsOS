all: archiver
archiver: main.o Vector.o File.o Archiver.o
	gcc main.o Vector.o File.o Archiver.o -o archiver
main.o: main.c
	gcc -c main.c
Vector.o: Vector.c
	gcc -c Vector.c
File.o: File.c
	gcc -c File.c
Archiver.o: Archiver.c
	gcc -c Archiver.c
clean:
	rm -rf *.o archiver
