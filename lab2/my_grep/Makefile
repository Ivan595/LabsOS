all: my_grep
my_grep: main.o Grep.o
	gcc main.o Grep.o -o my_grep
main.o: main.c
	gcc -c main.c
Grep.o: Grep.c
	gcc -c Grep.c
clean:
	rm -rf *.o my_grep
