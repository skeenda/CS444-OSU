#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

pthread_t searchers[2];
pthread_t inserters[2];
pthread_t deleters[2];

sem_t reading;
sem_t inserting;
sem_t deleting;

int curr_searchers = 0;
void* search(void *arg){
	int deleting_val;

	while(1){
		sem_getvalue(&deleting, &deleting_val);
		if(deleting_val <= 0){
			sem_wait(&deleting);
			sem_post(&deleting);
		}

		curr_searchers++;
		if(curr_searchers == 1){
			sem_wait(&reading);
		}

		printf("Searching\n");
		sleep(3);
		printf("done searching\n");

		if(curr_searchers == 1){
			sem_post(&reading);
		}
		curr_searchers--;


		sleep(4);
	}
}

void* insert(void *arg){
	while(1){
		sem_wait(&inserting);

		printf("inserting\n");
		sleep(3);
		printf("done inserting\n");

		sem_post(&inserting);

		sleep(1);
	}
}

void* delete(void *arg){
	while(1){
		sem_wait(&reading);
		sem_wait(&inserting);
		sem_wait(&deleting);

		printf("deleting\n");
		sleep(3);
		printf("done deleting\n");

		sem_post(&deleting);
		sem_post(&inserting);
		sem_post(&reading);

		sleep(1);
	}
}

int main(){
	int i;

	/* initialize semaphores */
	sem_init(&reading, 0, 1);
	sem_init(&inserting, 0, 1);
	sem_init(&deleting, 0, 1);

	/* spawn searcher threads */
	for(i=0; i < sizeof(searchers)/sizeof(searchers[0]); i++){
		pthread_create(&searchers[i], NULL, &search, NULL);
	}

	/* spawn searcher threads */
	for(i=0; i < sizeof(inserters)/sizeof(inserters[0]); i++){
		pthread_create(&inserters[i], NULL, &insert, NULL);
	}

	/* spawn deleter threads */
	for(i=0; i < sizeof(deleters)/sizeof(deleters[0]); i++){
		pthread_create(&deleters[i], NULL, &delete, NULL);
	}

	/* wait for searcher threads to finish (never) */
	for(i=0; i < sizeof(searchers)/sizeof(searchers[0]); i++){
		pthread_join(searchers[i], NULL);
	}
	
	/* wait for inserter threads to finish (never) */
	for(i=0; i < sizeof(inserters)/sizeof(inserters[0]); i++){
		pthread_join(inserters[i], NULL);
	}

	/* wait for deleter threads to finish (never) */
	for(i=0; i < sizeof(deleters)/sizeof(deleters[0]); i++){
		pthread_join(deleters[i], NULL);
	}
}
