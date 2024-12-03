all: sender reciever
sender: sender.o
	gcc sender.o -o sender
sender.o: sender.c
	gcc -c sender.c
reciever: reciever.o
	gcc reciever.o -o reciever
reciever.o:	reciever.c
	gcc -c reciever.c
clean:
	rm -rf *.o sender reciever
