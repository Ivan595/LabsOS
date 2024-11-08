all: recieve get
recieve: fifo_recieve.o
	gcc fifo_recieve.o -o recieve
fifo_recieve.o: fifo_recieve.c
	gcc -c fifo_recieve.c
get: fifo_get.o
	gcc fifo_get.o -o get
fifo_get.o: fifo_get.c
	gcc -c fifo_get.c
clean:
	rm -rf *.o recieve get
