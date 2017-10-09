#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <pthread.h>
//#include <string.h>
#include <semaphore.h>
#include <sys/queue.h>
#include <cpuid.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 32

pthread_t prod_id[2];
pthread_t con_id[2];

sem_t buffer_change_sem; /*semaphore to block when changes are being made to the buffer*/
sem_t buffer_full_sem;	 /*semaphore to block producers when the buffer is full (32)*/
sem_t buffer_empty_sem;	 /*semaphore to block consumers when buffer is empty*/

_Bool support_RDRAND;	 /*true = RDRAND is supposed, false = not supported*/

TAILQ_HEAD(head, event) head; /*head address of queue*/

/*--------------------------------------------------------------------
 *struct for events
--------------------------------------------------------------------*/
struct event
{
	int num;
	int random_wait;

	TAILQ_ENTRY(event) entries; /*linked list functionality for event*/
};


/*--------------------------------------------------------------------
 *Determines if the system supports RDRAND by reading cpu flags
--------------------------------------------------------------------*/
_Bool supports_RDRAND()
{	/*generate bit pattern expect if RDRAND is supported*/
	const unsigned int flag_RDRAND = (1 << 30); 
						     
	/*ecx contains the bit flag that we want*/
	unsigned int eax, ebx, ecx, edx; 
	/*Grab bit patterns from cpu*/
	__get_cpuid(1, &eax, &ebx, &ecx, &edx); 

	/*check if bit pattern from cpu matches generated pattern*/
	return ((ecx & flag_RDRAND) == flag_RDRAND); 
}

/*------------------------------------------------------------
 *initialize random number generators
 *determines if system supports RDRAND or not
 *seeds Mersenne Twister if it is being used
------------------------------------------------------------*/
int init_random()
{
	support_RDRAND = supports_RDRAND();

	if(!support_RDRAND) {
		init_genrand(time(NULL)); //set seed for Mersenne Twister
	}
}

/*------------------------------------------------------------
 *Generates random number in specified range (inclusive)
------------------------------------------------------------*/
int random_gen(int min, int max)
{
	unsigned int generated_num;
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

/*-------------------------------------------------
 *Returns number of items currently in queue
 *only used for testing
-------------------------------------------------*/
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

/*------------------------------------------------
 *creates new events to place in the queue
 *blocks if another thread is using the queue
 *blocks if the queue is full
-------------------------------------------------*/
void* producer(void *arg)
{
	struct event *new_event; /*new event to be produced*/
	int prod_time; /*time to spend producing*/

	while(1){
		prod_time = random_gen(3, 7);
		printf("Producing for %d seconds\n", prod_time);
		sleep(prod_time);

		/*create and fill event*/
		new_event = malloc(sizeof(struct event));
		new_event->num = random_gen(0, 10);
		new_event->random_wait = random_gen(2, 9);

		sem_wait(&buffer_full_sem); /*check if queue is full*/
		sem_wait(&buffer_change_sem); /*check if queue is in use*/

		printf("Produced %d after %d seconds\n",
			new_event->num,
			prod_time);

		TAILQ_INSERT_TAIL(&head, new_event, entries);

		/*increment semaphore to reflect additional event in queue*/
		sem_post(&buffer_empty_sem);
		sem_post(&buffer_change_sem);
	}
}

/*------------------------------------------------
 *Consumes events from the queue
 *blocks if another thread is using queue
 *blocks if queue is empty
-------------------------------------------------*/
void* consumer(void *arg)
{
	struct event *consumed_event;

	while(1){
		sem_wait(&buffer_empty_sem); /*check if queue is empty*/
		sem_wait(&buffer_change_sem); /*check if queue is in use*/

		consumed_event = head.tqh_first; /*get element at head of queue*/

		TAILQ_REMOVE(&head, consumed_event, entries);

		/*increment semaphore to reflect additional space in queue*/
		sem_post(&buffer_full_sem);
		sem_post(&buffer_change_sem);

		/*consume event*/
		printf("Consuming for %d seconds\n",
			consumed_event->random_wait);
		sleep(consumed_event->random_wait);
		printf("Consumed %d after %d seconds\n",
			consumed_event->num,
			consumed_event->random_wait);

		free(consumed_event);
	}
}

int main()
{
	int i;

	init_random(); /*initialize random generation*/

	TAILQ_INIT(&head); /*initialize queue data structure*/

	/*initialize sem to block after 1 wait*/
	sem_init(&buffer_change_sem, 0, 1); 

	/*initialize sem to MAX_BUFFER_SIZE
 	 *indicates that the queue is full
	 */	 	
	sem_init(&buffer_full_sem, 0, MAX_BUFFER_SIZE);

	/*initialize sem to 0
 	 *indicates that queue is empty
	 */
	sem_init(&buffer_empty_sem, 0, 0);

	/*spawn producer threads*/
	i = 0;
	while(i < 2){
		pthread_create(&(prod_id[i]), NULL, &producer, NULL);
		i++;
	}

	/*spawn consumer threads*/
	i = 0;
	while(i < 2){
		pthread_create(&(con_id[i]), NULL, &consumer, NULL);
		i++;
	}

	/*wait until all threads die*/
	for(i = 0; i < sizeof(prod_id)/sizeof(prod_id[0]); i++){
		pthread_join(prod_id[i], NULL);
	}
	for(i = 0; i < sizeof(con_id)/sizeof(con_id[0]); i++){
		pthread_join(con_id[i], NULL);
	}
}
