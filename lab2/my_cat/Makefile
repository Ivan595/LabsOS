all: my_cat
my_cat: main.o Cat.o
	gcc main.o Cat.o -o my_cat
main.o: main.c
	gcc -c main.c
Cat.o: Cat.c
	gcc -c Cat.c
clean:
	rm -rf *.o my_cat
