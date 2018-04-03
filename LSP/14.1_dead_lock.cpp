/*
 * 14.1_dead_lock.cpp
 * Author: Andrea
 * Note: 
 * Created Time: 2018年04月03日 星期二 14时30分03秒
 */

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

int a = 1;
int b = 10;
pthread_mutex_t mutex_a;
pthread_mutex_t mutex_b;

void* another(void* arg) {
	pthread_mutex_lock(&mutex_b);
	printf("in child thread, got mutex b\n");
	sleep(5);
	++b;
	pthread_mutex_lock(&mutex_a);
	printf("in child thread, got mutex a\n");
	b += a++;
	pthread_mutex_unlock(&mutex_a);
	pthread_mutex_unlock(&mutex_b);
	pthread_exit(NULL);
}

int main() {
	pthread_t id;

	pthread_mutex_init(&mutex_a, NULL);
	pthread_mutex_init(&mutex_b, NULL);
	pthread_create(&id, NULL, another, NULL);

	pthread_mutex_lock(&mutex_a);
	printf("in parent thread, got mutex a\n");
	sleep(5);
	++a;
	pthread_mutex_lock(&mutex_b);
	a += b++;
	pthread_mutex_unlock(&mutex_b);
	pthread_mutex_unlock(&mutex_a);

	pthread_join(id, NULL);
	pthread_mutex_destroy(&mutex_a);
	pthread_mutex_destroy(&mutex_b);
	return 0;
}
