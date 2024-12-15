#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char output[] = "ACCEPT THE LAB PLEASE";
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

//create shared resources
int size;
int capacity;
char* array;

void* writer() {
    usleep(1);
    while (size <= strlen(output)) {
        pthread_rwlock_wrlock(&rwlock);   

        //push data into array
        if (size == capacity) {
            capacity *= 2;
            array = (char*)realloc(array, sizeof(char) * capacity);
        }

        array[size] = output[size];
        size += 1;

        usleep(200000); //Имитация бурной деятельности
        pthread_rwlock_unlock(&rwlock);
        usleep(100000);
    }
    return NULL;
}

void* reader() {
    while (size <= strlen(output)) {
        pthread_rwlock_rdlock(&rwlock);

        //read data from array
        char* current_array = (char*)calloc(size + 1, sizeof(char));
        strncpy(current_array, array, size);
        printf("my_tid = %lu\ncurrent state: %s\n\n", pthread_self(), current_array);
        free(current_array);

        pthread_rwlock_unlock(&rwlock);
        usleep(200000);
    }
    return NULL;
}

int main() {

    //initialize shared resources
    size = 1;
    capacity = 1;
    array = (char*)calloc(capacity, sizeof(char));
    array[0] = output[0];

    pthread_t threads[11];
    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, reader, NULL);
    }
    pthread_create(&threads[10], NULL, writer, NULL);

    for (int i = 0; i < 11; i++) {
        pthread_join(threads[i], NULL);
    }

    free(array);
    return 0;
}
