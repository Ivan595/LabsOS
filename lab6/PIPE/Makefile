all: pipe
pipe: pipe.o
	gcc pipe.o -o pipe
pipe.o: pipe.c
	gcc -c pipe.c
clean:
	rm -rf *.o pipe
run:
	./pipe
