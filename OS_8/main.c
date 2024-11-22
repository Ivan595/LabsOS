#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx;

void* func(void* arg) {

	usleep(1000);
	
	if (pthread_mutex_trylock(&mtx) == 0) {

		for (int i = 0; i < 5; i++) {
			printf("thread[%x] %d\n", pthread_self(), i);
			usleep(100000);
		}
		pthread_mutex_unlock(&mtx);
	}
	else {
		printf("Mutex is busy\n");
	}	

	pthread_exit(NULL);	

}



int main() {

	pthread_mutex_init(&mtx, NULL);
	printf("hello from main\n");

	pthread_t threads[5];
	

	for (int i = 0; i < 10; i++) {
		
		int create_res = pthread_create(&threads[i], NULL, func, NULL);

	}
	sleep(1);
	for (int i = 0; i < 5; i++) {
		void* res = NULL;
		int join_res = pthread_join(threads[i], &res);
	}
	
	pthread_mutex_destroy(&mtx);
	return 0;


}
