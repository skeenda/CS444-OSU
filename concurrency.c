#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <sys/queue.h>

#define MAX_BUFFER_SIZE 32

pthread_t prod_id[2];
pthread_t con_id[2];
sem_t buffer_change_sem;	//semaphore to block when changes are being made to the buffer
sem_t buffer_full_sem;		//semaphore to block producers when the buffer is full (32)
sem_t buffer_empty_sem;		//semaphore to block consumers when buffer is empty

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
	struct event *new_event;

	new_event = malloc(sizeof(struct event));
	new_event->num = genrand_int32() % 8 + 2;
	new_event->random_wait = 5;

	sem_wait(&buffer_full_sem);
	sem_wait(&buffer_change_sem);

	printf("Producing: %d, %d\n", new_event->num, new_event->random_wait);

	TAILQ_INSERT_TAIL(&head, new_event, entries);

	printf("size: %d\n", queue_size());

	sem_post(&buffer_empty_sem);
	sem_post(&buffer_change_sem);
}

void* consumer(void *arg)
{
	struct event *consumed_event;

	sem_wait(&buffer_empty_sem);
	sem_wait(&buffer_change_sem);


	consumed_event = head.tqh_first;
	
	printf("Consuming: %d, %d\n", consumed_event->num, consumed_event->random_wait);

	TAILQ_REMOVE(&head, consumed_event, entries);

	printf("Size: %d\n", queue_size());

	sem_post(&buffer_full_sem);
	sem_post(&buffer_change_sem);
}

int main()
{
	TAILQ_INIT(&head);

	sem_init(&buffer_change_sem, 0, 1);
	sem_init(&buffer_full_sem, 0, MAX_BUFFER_SIZE);
	sem_init(&buffer_empty_sem, 0, 0);

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
