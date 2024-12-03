#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_SIZE 64
#define DATA_LEN 64

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int semid, shmid;
char *addr;

void sigint_handler(int signum) {
    shmdt(addr);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    exit(0);
}

int main() {

    signal(SIGINT, sigint_handler);
    key_t key = ftok(".", 21);

    union semun arg;
    semid = semget(key, 1, 0600 | IPC_CREAT | IPC_EXCL);
	shmid = shmget(key, SHM_SIZE, 0600 | IPC_CREAT | IPC_EXCL);
	if (shmid == -1) {
		int err = errno;
		printf("shmget %s (%d)\n", strerror(err), err);
		return -1;
	}
    if (semid == -1) {
        int err = errno;
        printf("semget %s (%d)\n", strerror(err), err);
        return -1;
    }

    addr = (char*)shmat(shmid, NULL, 0);
	if (addr == (char*)-1) {		
		int err = errno;
		printf("shmat %s (%d)\n", strerror(err), err);
		return -1;
	}
    
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        int err = errno;
        printf("semctl %s (%d)\n", strerror(err), err);
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

        //get data
		time_t s_time = time(NULL);
		struct tm m_time;
		localtime_r(&s_time, &m_time);
		char* data = (char*)calloc(DATA_LEN, sizeof(char));
		sprintf(data, "[SENDER]: TIME %d:%d:%d PID %d", m_time.tm_hour, m_time.tm_min, m_time.tm_sec, getpid());
        
        memcpy(addr, data, strlen(data) + 1);
        free(data);

        if (semop(semid, &unlock, 1) == -1) {
            int err = errno;
            printf("semop %s (%d)\n", strerror(err), err);
            return -1;
        }
        usleep(1);
    }


    return 0;
}


