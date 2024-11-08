#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#define DATE_LEN 9

int main(int argc, char** argv) {
    
    (void)argc;
    (void)argv;

    int res = 0;
    int pipedesc[2];
    int pipeRes = pipe(pipedesc);
    if (pipeRes != 0) {
        int err = errno;
        fprintf(stderr, "Error: %s (%d)\n", strerror(err), err);
        return 1;
    }

    switch (res = fork()) {
        case -1:
        {
            int err = errno;
            fprintf(stderr, "Fork error: %s (%d)\n", strerror(err), err);
            break;
        }
        case 0:
        {
            //child process
            
            //sleep for 5 seconds
            usleep(5000000);
            time_t s_time = time(NULL);
            struct tm m_time;
            localtime_r(&s_time, &m_time);
            char* current_time = (char*)calloc(DATE_LEN, sizeof(char));
            sprintf(current_time, "%d:%d:%d", m_time.tm_hour, m_time.tm_min, m_time.tm_sec);
            printf("[CHILD]: time = %s\n", current_time);
            
            //get transmitted_data
            char buf[DATE_LEN];
            pid_t transmitting_process;
            close(pipedesc[1]);
            read(pipedesc[0], buf, DATE_LEN);
            read(pipedesc[0], &transmitting_process, sizeof(pid_t));
            close(pipedesc[0]);

            printf("[CHILD]: transmitted_time = %s\n", buf);
            printf("[CHILD]: id of transmitting_process = %d\n", transmitting_process);
            break;
        }
        default:
        {
            //parent process
            
            int childRes;

            //get time
            time_t s_time = time(NULL);
            struct tm m_time;
            localtime_r(&s_time, &m_time);
            char* current_time = (char*)calloc(DATE_LEN, sizeof(char));
            sprintf(current_time, "%d:%d:%d", m_time.tm_hour, m_time.tm_min, m_time.tm_sec);
            pid_t pid = getpid();
            printf("[PARENT]: time = %s\n", current_time);
            printf("[PARENT]: pid = %d\n", pid);
            
            close(pipedesc[0]);
            write(pipedesc[1], current_time, DATE_LEN);
            write(pipedesc[1], &pid, sizeof(pid_t));
            close(pipedesc[1]);

            wait(&childRes);
            break;
        }
    }

    return 0;
}
