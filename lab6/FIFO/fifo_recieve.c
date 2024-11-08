#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#define DATE_LEN 9

int main(int argc, char** argv) {
    
    if (argc != 2) {
        fprintf(stderr, "Wrong args\n");
        return 1;
    }

    //if file already exists then delete it
    unlink(argv[1]);

    if (mkfifo(argv[1], S_IRUSR | S_IWUSR) == -1) {
        int err = errno;
        fprintf(stderr, "Error: %s (%d)\n", strerror(err), err);
        return 1;
    }

    //open fifo for writing
    int fd_fifo;
    if ((fd_fifo = open(argv[1], O_WRONLY)) == -1) {
        int err = errno;
        fprintf(stderr, "Erorr: %s (%d)\n", strerror(err), err);
        return 1;
    }   


    
    time_t s_time = time(NULL);
    struct tm m_time;
    localtime_r(&s_time, &m_time);
    char* current_time = (char*)calloc(DATE_LEN, sizeof(char));
    sprintf(current_time, "%d:%d:%d", m_time.tm_hour, m_time.tm_min, m_time.tm_sec);
    pid_t pid = getpid();

    printf("[RECEIVING PROCESS]: time = %s\n", current_time);
    printf("[RECEIVING PROCESS]: pid = %d\n", pid);
    //wait untill another process opens fifo for reading
    write(fd_fifo, current_time, DATE_LEN);
    write(fd_fifo, &pid, sizeof(pid_t));

    unlink(argv[1]);        
    return 0;

}
