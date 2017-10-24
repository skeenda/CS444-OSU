#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

pthread_t philo_id[5];

void* table(void *arg){
	printf("I think therefore I am\n");
}

int main(){
	pthread_create(&philo_id[0], NULL, &table, NULL);

	pthread_join(philo_id[0], NULL);
}

