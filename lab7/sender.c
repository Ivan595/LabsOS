#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

int main() {
    key_t key;
    if ((key = ftok("shm", 1)) == -1) {
        int err = errno;
        printf("ftok %s (%d)\n", strerror(err), err);
        return -1;
    }   
    int shmid = shmget(key, 64, 0666 | IPC_CREAT);
    if (shmid < 0) {
        int err = errno;
        printf("shmget %s (%d)\n", strerror(err), err);
        return -1;
    }
    int* addr = (int*)shmat(shmid, NULL, 0);
    if (addr == (void*)-1) {
        int err = errno;
        printf("shmat %s (%d)\n", strerror(err), err);
        return -1;
    }       

    int res = shmdt(addr);
    if (res == -1) {
        int err = errno;
        printf("shmdt %s (%d)\n", strerror(err), err);
        return -1;
    }
    
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        int err = errno;
        printf("shmctl %s (%d)\n", strerror(err), err);
        return -1;
    }


    return 0;

}
