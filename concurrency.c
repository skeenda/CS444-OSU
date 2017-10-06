#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <sys/queue.h>

pthread_t tid[2];
sem_t mySem;

struct event
{
	int num;
	int random_wait;

	TAILQ_ENTRY(event) entries;
};

TAILQ_HEAD(head, event) head;

void* producer(void *arg)
{
	sem_wait(&mySem);

	struct event *myEvent = malloc(sizeof(struct event));
	myEvent->num = genrand_int32() % 8 + 2;
	myEvent->random_wait = 5;

	TAILQ_INSERT_TAIL(&head, myEvent, entries);

	sem_post(&mySem);
}

void* consumer(void *arg)
{
	struct event *np;

	sem_wait(&mySem);

	for(np = head.tqh_first; np != NULL; np = np->entries.tqe_next){
		printf("%d, %d\n", np->num, np->random_wait);
	}

	sem_post(&mySem);
}

int main()
{
	TAILQ_INIT(&head);
	
	sem_init(&mySem, 0, 1);

	init_genrand(time(NULL));

	int i = 0;
	while(i < 2){
		pthread_create(&(tid[i]), NULL, &producer, NULL);
		i++;
	}

	pthread_t consumerID;
	pthread_create(&consumerID, NULL, &consumer, NULL);


	for(i=0; i<1000000;i++){}
}
