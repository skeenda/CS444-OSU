#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

pthread_t searchers[2];
pthread_t inserters[2];
pthread_t deleters[2];

sem_t reading;
sem_t inserting;
sem_t deleting;

int active_searchers = 0;
int active_inserters = 0;
int active_deleters = 0;

int curr_searchers = 0;

/* prints current # of active searchers, inserters, and deleters */
void print_status(){
	printf("%d\t\t%d\t\t%d\n",
		active_searchers, active_inserters, active_deleters);
}

/* any # of readers can read at a time
 * cannot run when a deleter is running */
void* search(void *arg){
	int deleting_val;

	while(1){
		/* if deleter is running searchers must wait for it to finish 
 		 * immdetiently post to allow other searchers through  */
		sem_getvalue(&deleting, &deleting_val);
		if(deleting_val <= 0){
			sem_wait(&deleting);
			sem_post(&deleting);
		}

		/* only the first searcher should wait on the semaphore
 		 * this is used to block the deleters */
		curr_searchers++;
		if(curr_searchers == 1){
			sem_wait(&reading);
		}

		active_searchers++;
		print_status();
		sleep(3);
		active_searchers--;
		print_status();

		if(curr_searchers == 1){
			sem_post(&reading);
		}
		curr_searchers--;


		sleep(4);
	}
}

/* only 1 inserter can run at a time
 * no deleters
 * any # of readers */
void* insert(void *arg){
	while(1){
		sem_wait(&inserting);

		active_inserters++;
		print_status();
		sleep(3);
		active_inserters--;
		print_status();

		sem_post(&inserting);

		sleep(1);
	}
}

/* only 1 deleter and nothing else can run at a time */
void* delete(void *arg){
	while(1){
		sem_wait(&reading);
		sem_wait(&inserting);
		sem_wait(&deleting);

		active_deleters++;
		print_status();
		sleep(3);
		active_deleters--;
		print_status();

		sem_post(&deleting);
		sem_post(&inserting);
		sem_post(&reading);

		sleep(1);
	}
}

int main(){
	int i;

	printf("searchers\tinserters\tdeleters\n");

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
