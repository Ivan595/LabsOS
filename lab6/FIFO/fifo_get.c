#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define DATE_LEN 9


int main(int argc, char** argv) {
    
    if (argc != 2) {
        fprintf(stderr, "Wrong args\n");
        return 1;
    }
    
    //open fifo for reading
    int fd_fifo;
    if ((fd_fifo = open(argv[1], O_RDONLY)) == -1) {
        int err = errno;
        fprintf(stderr, "Erorr: %s (%d)\n", strerror(err), err);
        return 1;
    }   
    
    //sleep for 5 seconds
    usleep(5000000);
    time_t s_time = time(NULL);
    struct tm m_time;
    localtime_r(&s_time, &m_time);
    char* current_time = (char*)calloc(DATE_LEN, sizeof(char));
    sprintf(current_time, "%d:%d:%d", m_time.tm_hour, m_time.tm_min, m_time.tm_sec);
    printf("[GETTING PROCESS]: time = %s\n", current_time);

    //get transmitted_data
    char buf[DATE_LEN];
    pid_t transmitting_process;
    read(fd_fifo, buf, DATE_LEN);
    read(fd_fifo, &transmitting_process, sizeof(pid_t));

    printf("[GETTING PROCESS]: transmitted_time = %s\n", buf);
    printf("[GETTING PROCESS]: id of transmitting_process = %d\n", transmitting_process);

    return 0;
}
