#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_mutex_t mtx;

typedef enum
{
    WRITE,
    READ
} thread_type;


//create shared resources
int size;
int capacity;
int* array;

void my_read() {
    printf("my_tid = %lu\n", pthread_self());
    printf("current state: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    printf("\n");
}

void my_write() {
    if (size == capacity) {
        capacity *= 2;
        array = (int*)realloc(array, sizeof(int) * capacity);
    }
    array[size] = size;
    size += 1;
}

void* manage_array(void* arg) {
    if (*(thread_type*)arg == WRITE) {
        usleep(1000);
    }   
    while (1) {
        pthread_mutex_lock(&mtx);
        if (*(thread_type*)arg == READ) {
            my_read();
        }
        else {
            my_write();
        }
        usleep(100000);
        pthread_mutex_unlock(&mtx);
        usleep(1000);
    }
}


void sigint_handler(int signum) {
    free(array);
    pthread_mutex_destroy(&mtx);
    exit(0);
}

int main() {
    signal(SIGINT, sigint_handler);
	pthread_mutex_init(&mtx, NULL);

    //initialize shared resources
    size = 1;
    capacity = 1;
    array = (int*)calloc(capacity, sizeof(int));

    thread_type arg1, arg2;
    arg1 = READ;
    arg2 = WRITE;

	
    printf("hello from main\n");
	pthread_t threads[11];
	
	for (int i = 0; i < 10; i++) {
        //create reading threads
	    pthread_create(&threads[i], NULL, manage_array, &arg1);
	}
    //create writing thread
    pthread_create(&threads[10], NULL, manage_array, &arg2);
    
	usleep(1000000);
	for (int i = 0; i < 11; i++) {
		void* res = NULL;
		pthread_join(threads[i], &res);
	}

	return 0;	    
}
