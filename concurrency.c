#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <sys/queue.h>
#include <cpuid.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 32

pthread_t prod_id[2];
pthread_t con_id[2];
sem_t buffer_change_sem;	//semaphore to block when changes are being made to the buffer
sem_t buffer_full_sem;		//semaphore to block producers when the buffer is full (32)
sem_t buffer_empty_sem;		//semaphore to block consumers when buffer is empty

_Bool support_RDRAND;

struct event
{
	int num;
	int random_wait;

	TAILQ_ENTRY(event) entries;
};

TAILQ_HEAD(head, event) head;

_Bool supports_RDRAND()
{
	const unsigned int flag_RDRAND = (1 << 30);

	unsigned int eax, ebx, ecx, edx;
	__get_cpuid(1, &eax, &ebx, &ecx, &edx);

	return ((ecx & flag_RDRAND) == flag_RDRAND);
}

int init_random()
{
	support_RDRAND = supports_RDRAND();

	if(!support_RDRAND) {
		init_genrand(time(NULL)); //set seed for Mersenne Twister
	}
}

int random_gen(int min, int max)
{
	int generated_num;
	char rc;

	int actual_max = max - min + 1;

	if(!support_RDRAND){
		generated_num = genrand_int32() % actual_max + min;
	}
	else{
		do{
			__asm__ volatile(
				"rdrand %0 ; setc %1"
				: "=r" (generated_num), "=qm" (rc)
			);
		}while(!rc);

		generated_num = generated_num % actual_max + min;
	}

	return generated_num;
}

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

	sleep(random_gen(3, 7));

	new_event = malloc(sizeof(struct event));
	new_event->num = random_gen(0, 10);
	new_event->random_wait = random_gen(2, 9);

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

	sleep(consumed_event->random_wait);
	
	printf("Consuming: %d, %d\n", consumed_event->num, consumed_event->random_wait);

	TAILQ_REMOVE(&head, consumed_event, entries);

	printf("Size: %d\n", queue_size());

	sem_post(&buffer_full_sem);
	sem_post(&buffer_change_sem);
}

int main()
{
	init_random();

	TAILQ_INIT(&head);

	sem_init(&buffer_change_sem, 0, 1);
	sem_init(&buffer_full_sem, 0, MAX_BUFFER_SIZE);
	sem_init(&buffer_empty_sem, 0, 0);


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


	for(i = 0; i < sizeof(prod_id)/sizeof(prod_id[0]); i++){
		pthread_join(prod_id[i], NULL);
	}
	for(i = 0; i < sizeof(con_id)/sizeof(con_id[0]); i++){
		pthread_join(con_id[i], NULL);
	}
}
