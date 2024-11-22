#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#define DATA_LEN 64

char* addr;
void sigint_handler(int signum) {
    int res = shmdt(addr);
    exit(0);
}

int main() {
    signal(SIGINT, sigint_handler);
	key_t key = ftok("shared_memory", 20);	
	int shmid = shmget(key, 64, 0666);
	if (shmid < 0) {
		int err = errno;
		printf("shmget %s (%d)\n", strerror(err), err);
		return -1;
	}
	
	addr = (char*)shmat(shmid, NULL, 0);
	if (addr == (char*)-1) {		
		int err = errno;
		printf("shmat %s (%d)\n", strerror(err), err);
		return -1;
	}
    
    while (1) {
		//get data
		time_t s_time = time(NULL);
		struct tm m_time;
		localtime_r(&s_time, &m_time);
		char* data = (char*)calloc(DATA_LEN, sizeof(char));
		sprintf(data, "[RECIEVER]: TIME %d:%d:%d PID %d", m_time.tm_hour, m_time.tm_min, m_time.tm_sec, getpid());
        printf("%s ", data);
        printf("%s\n", addr);
        
        free(data);
        sleep(1);
    }
}




