#include <sys/sem.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define SHM_SIZE 64
#define DATA_LEN 64

char *addr;
void sigint_handler(int signum) {
    shmdt(addr);
    exit(0);
}

int main() {
    signal(SIGINT, sigint_handler);
    
    key_t key = ftok(".", 21);

    int semid = semget(key, 1, 0600);
    if (semid == -1) {
        int err = errno;
        printf("semget %s (%d)\n", strerror(err), err);
        return -1;
    }

    int shmid = shmget(key, SHM_SIZE, 0600);
	if (shmid == -1) {
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
    
    struct sembuf lock;
    lock.sem_num = 0;
    lock.sem_op = -1;
    lock.sem_flg = 0;
    
    struct sembuf unlock;
    unlock.sem_num = 0;
    unlock.sem_op = 1;
    unlock.sem_flg = 0;

    while (1) {


        if (semop(semid, &lock, 1) == -1) {
            int err = errno;
            printf("semop %s (%d)\n", strerror(err), err);
            return -1;
        }

        for (int i = 0; i < 3; i++) {
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

        if (semop(semid, &unlock, 1) == -1) {
            int err = errno;
            printf("semop %s (%d)\n", strerror(err), err);
            return -1;
        }
        usleep(1);
    }


    return 0;
}
