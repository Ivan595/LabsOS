all: test
test: cond_var.o
	gcc cond_var.o -pthread -o test
cond_var.o: cond_var.c
	gcc -c cond_var.c -pthread
clean:
	rm -rf *.o test
