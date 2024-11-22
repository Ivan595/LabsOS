#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#define DATE_LEN 9
#define PID_LEN 5

int main() {
	
	key_t key = ftok("shm", 5);	

	//на основе этих флагов проверять IPC_CREATE or smth проверять если уже что то запущено
	int shmid = shmget(key, 64, 0666 | IPC_CREAT | IPC_EXCL);
	
	if (shmid < 0) {
		int err = errno;
		printf("shmget %s (%d)\n", strerror(err), err);
		return -1;
	}
	
	char* addr = (char*)shmat(shmid, NULL, 0);
	if (addr == (char*)-1) {		
		int err = errno;
		printf("shmat %s (%d)\n", strerror(err), err);
		return -1;
	}

	int count = 0;
	while (count < 60) {
		//get time
		sleep(1);
		time_t s_time = time(NULL);
		struct tm m_time;
		localtime_r(&s_time, &m_time);
		char* current_time = (char*)calloc(DATE_LEN, sizeof(char));
		sprintf(current_time, "%d:%d:%d", m_time.tm_hour, m_time.tm_min, m_time.tm_sec);
		pid_t pid = getpid();
		char* pid_str = (char*)calloc(PID_LEN, sizeof(char));
		sprintf(str, "%d", pid);
		strcpy(addr, current_time);
		strcpy(addr, pid_str);
		free(current_time);
		free(pid_str);
		count++;
	}
	
	int res = shmdt(addr);
	shmctl(shmid, IPC_RMID, NULL);

}
