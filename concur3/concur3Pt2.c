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

int main(){
	int i;

	for(i=0; i < sizeof(searchers)/sizeof(searchers[0]); i++){
		pthread_create(&searchers[i], NULL, &search, NULL);
	}

	for(i=0; i < sizeof(searchers)/sizeof(searchers[0]); i++){
		pthread_join(searchers[i], NULL);
	}
}
