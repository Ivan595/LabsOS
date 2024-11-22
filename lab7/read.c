#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

int main() {
	
	key_t key = ftok("shm", 5);	

	//на основе этих флагов проверять IPC_CREATE or smth проверять если уже что то запущено
	int shmid = shmget(key, 64, 0666);
	
	if (shmid < 0) {
		int err = errno;
		printf("shmget %s (%d)\n", strerror(err), err);
		return -1;
	}
	
	int* addr = shmat(shmid, NULL, 0);
	if (addr == (void*)-1) {		
		int err = errno;
		printf("shmat %s (%d)\n", strerror(err), err);
		return -1;
	}

	//проверки на errno
	
	for (int i = 0; i < 16; i++) 
		printf("%d\n", addr[i]);
	
	int res = shmdt(addr);


}
