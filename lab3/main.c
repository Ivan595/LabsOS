#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pwd.h>

void common_exit() {
    printf("COMMON EXIT\n");    
}

void child_exit() {
    printf("CHILD EXIT\n");
}

void parent_exit(int status, void* arg) {
    printf("PARENT EXIT\texit code: %d\tmessage: %s\n", status, (char*)arg);
    free(arg);
}

void sigint_handler(int signum) {
	printf("Signal %d recieved\n", signum);
}

void sigterm_handler(int signum, siginfo_t* info, void* context) {
    printf("Signal %d recieved\n", signum);
    printf("Reciever PID: %d\n", info->si_pid);
    struct passwd* ptr;
    if ((ptr = getpwuid(info->si_uid)) == NULL) {
        printf("Reciever username: %u\n", (unsigned int)info->si_uid);
    }
    else {
        printf("Reciever username: %s\n", ptr->pw_name);
    }
}

int main(int argc, char** argv) {

    struct sigaction sa;
    sa.sa_sigaction = sigterm_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, NULL);

    atexit(common_exit);

    pid_t res;

	switch(res = fork()) {
		case -1: {
			fprintf(stderr, "Fork error: %s (%d)\n", strerror(errno), errno);
			break;
		}
		case 0: {
            signal(SIGINT, sigint_handler);
            atexit(child_exit);
            sleep(2);
			printf("[CHILD] my pid is %d\tmy parent pid is %d\n", getpid(), getppid());
			break;
		}
		default: {
			signal(SIGINT, SIG_IGN);
            char* buf = (char*)calloc(50, sizeof(char));
            sprintf(buf, "closed process with pid %d", getpid());
            on_exit(parent_exit, buf);
			int ch_res;
			wait(&ch_res);
            sleep(20);
			printf("[PARENT] my pid is %d\tmy parent pid is %d\tmy child pid is %d\n",
                    getpid(), getppid(), res);
			printf("[PARENT} my child exit code is %d\n", WEXITSTATUS(ch_res));
            //exit(3); - продемонстрировать вывод статуса завершения процесса (on_exit)
			break;
		}
	}
	return 0;
}
