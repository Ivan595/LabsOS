all: test
test: mutex.o
	gcc mutex.o -pthread -o test
mutex.o: mutex.c
	gcc -c mutex.c -pthread
clean:
	rm -rf *.o test
