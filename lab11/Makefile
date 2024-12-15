all: test
test: rwlock.o
	gcc rwlock.o -lpthread -o test
rwlock.o: rwlock.c
	gcc -c rwlock.c -lpthread
clean:
	rm -rf *.o test
