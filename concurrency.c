#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <sys/queue.h>

pthread_t prod_id[2];
pthread_t con_id[2];
sem_t mySem;

struct event
{
	int num;
	int random_wait;

	TAILQ_ENTRY(event) entries;
};

TAILQ_HEAD(head, event) head;

int queue_size()
{
	struct event *np;
	int size = 0;

	for(np = head.tqh_first; np != NULL; np = np->entries.tqe_next)
	{
		size++;
	}

	return size;
}

void* producer(void *arg)
{
	sem_wait(&mySem);

	struct event *myEvent = malloc(sizeof(struct event));
	myEvent->num = genrand_int32() % 8 + 2;
	myEvent->random_wait = 5;

	printf("Producing: %d, %d\n", myEvent->num, myEvent->random_wait);

	TAILQ_INSERT_TAIL(&head, myEvent, entries);

	printf("size: %d\n", queue_size());

	sem_post(&mySem);
}

void* consumer(void *arg)
{
	struct event *consumed_event;
	sem_wait(&mySem);

	consumed_event = head.tqh_first;
	
	printf("Consuming: %d, %d\n", consumed_event->num, consumed_event->random_wait);
	TAILQ_REMOVE(&head, consumed_event, entries);

	printf("Size: %d\n", queue_size());

	sem_post(&mySem);
}

int main()
{
	TAILQ_INIT(&head);

	sem_init(&mySem, 0, 1);

	init_genrand(time(NULL));

	int i = 0;
	while(i < 2){
		pthread_create(&(prod_id[i]), NULL, &producer, NULL);
		i++;
	}

	i = 0;
	while(i < 2){
		pthread_create(&(con_id[i]), NULL, &consumer, NULL);
		i++;
	}


	for(i=0; i<1000000;i++){}
}
