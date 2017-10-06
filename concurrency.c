#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

pthread_t tid[2];
sem_t mySem;
struct event event_buffer[32];

struct event {
	int num;
	int random_wait;
};

void* doSomeThing(void *arg){
	sem_wait(&mySem);

	struct event myEvent;
	myEvent.num = genrand_int32();
	myEvent.randomWait = 5;
	
	event_buffer

	sem_post(&mySem);

	int i=0;
	for(i=0; i<(100000000000);i++);
}

int main(){

	sem_init(&mySem, 0, 1);

	int i = 0;
	while(i < 2){
		pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
		i++;
	}

	for(i=0; i<1000000;i++){}
}
