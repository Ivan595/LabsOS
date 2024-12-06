#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>

#include <pthread.h>

#define STORAGE_MAX 100
#define STORAGE_MIN 30

static int storage = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* consumerFunc(void* arg) {
	puts("[CONSUMER] thread started");
	int toConsume = 0;

	while(1) {
		pthread_mutex_lock(&mtx);
		while(storage < STORAGE_MAX) {
			pthread_cond_wait(&cond, &mtx);
		}
		toConsume = STORAGE_MIN;
		printf("[CONSUMER][%x] storage is max, consuming %d\n", pthread_self(), toConsume);
		storage -= toConsume;
		printf("[CONSUMER][%x] storage = %d\n", pthread_self(), storage);
		pthread_mutex_unlock(&mtx);
	}
	pthread_exit(NULL);
}

void* producerFunc(void* arg) {
	puts("[PRODUCER] thread started");

	while(1) {
		usleep(200000);
		pthread_mutex_lock(&mtx);
		storage += 5;
		printf("[PRODUCER][%x] storage = %d\n", pthread_self(), storage);
		if (storage >= STORAGE_MAX) {
			puts("[PRODUCER] storage maximum");
			pthread_cond_broadcast(&cond);
		}
		pthread_mutex_unlock(&mtx);
	}
	pthread_exit(NULL);
}


int main() {

	pthread_t producer;
	pthread_t consumers[5];

	int create_res = pthread_create(&producer, NULL, producerFunc, NULL);

	for (int i = 0; i < 5; i++) {
		int create_res = pthread_create(&consumers[i], NULL, consumerFunc, NULL);
	}

	int prod_join_res = pthread_join(producer, NULL);
	for (int i = 0; i < 5; i++) {
		void* res = NULL;
		int join_res = pthread_join(consumers[i], &res);
	}

	return 0;

}
