all: my_ls
my_ls: main.o Vector.o File.o Ls.o
	gcc main.o Vector.o File.o Ls.o -o my_ls
main.o: main.c
	gcc -c main.c
Vector.o: Vector.c
	gcc -c Vector.c
File.o: File.c
	gcc -c File.c
Ls.o: Ls.c
	gcc -c Ls.c
clean:
	rm -rf *.o my_ls
