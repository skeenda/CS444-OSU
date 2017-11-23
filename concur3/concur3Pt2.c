#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

pthread_t searchers[1];
pthread_t inserters[1];
pthread_t deleters[1];

sem_t inserting;
sem_t deleting;

void* search(void *arg){
	printf("I am a searcher.\n");
}

void* insert(void *arg){
	printf("I am a inserter.\n");
}

void* delete(void *arg){
	printf("I am a deleter.\n");
}

int main(){
	int i;

	/* initialize semaphores */
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
