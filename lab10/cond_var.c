#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#define READERS_NUM 10
#define WRITERS_NUM 1


//create mutex
pthread_mutex_t mtx;
//create condition variable
pthread_cond_t cond;

//create shared resources
int size;
int capacity;
int* array;

int remaining_reading_threads = READERS_NUM;

//have to do this because when i send SIGINT some threads are in pthread_cond_wait
//and after sigint (after sigint_handler) they try to read freed data
volatile bool shouldExit = false;

void* readerFunc(void* arg) {

	while(1) {
		
        pthread_mutex_lock(&mtx);

        while (remaining_reading_threads == 0) {
            pthread_cond_wait(&cond, &mtx);
        }
        
        
        if (shouldExit == true) {
            pthread_mutex_unlock(&mtx);
            pthread_exit(NULL);
        }
        

        //read data from array
        printf("my_tid = %lu\n", pthread_self());
        printf("current state: ");
        for (int i = 0; i < size; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
        printf("\n");
		
        //decrement the number of remaining threads
        remaining_reading_threads -= 1;
        
		
        pthread_mutex_unlock(&mtx);
        usleep(1000);

	}
}

void* writerFunc(void* arg) {

    while(1) {
        
        usleep(200000);
		pthread_mutex_lock(&mtx);

        remaining_reading_threads = READERS_NUM;

        //push data into array
        if (size == capacity) {
            capacity *= 2;
            array = (int*)realloc(array, sizeof(int) * capacity);
        }

        if (shouldExit == true) {
		    pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mtx);
            pthread_exit(NULL);
        }
        array[size] = size;
        size += 1;
         
        //send signal to the condition varible
		pthread_cond_broadcast(&cond);

        pthread_mutex_unlock(&mtx);
	}
}

void sigint_handler(int signum) {
    shouldExit = true;
    free(array);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);
    exit(0);
}

int main() {

    signal(SIGINT, sigint_handler);

    //initialize mutex
	pthread_mutex_init(&mtx, NULL);
    //initialize condition variable
    pthread_cond_init(&cond, NULL);

    //initialize shared resources
    size = 1;
    capacity = 1;
    array = (int*)calloc(capacity, sizeof(int));
    
    //create threads
	pthread_t threads[READERS_NUM + WRITERS_NUM];
	
    for (int i = 0; i < READERS_NUM; i++) {
        //create reading threads
	    pthread_create(&threads[i], NULL, readerFunc, NULL);
	}
    //create writing thread
    pthread_create(&threads[10], NULL, writerFunc, NULL);
    
	for (int i = 0; i < READERS_NUM + WRITERS_NUM; i++) {
		void* res = NULL;
		pthread_join(threads[i], &res);
	}

	return 0;	    
}
