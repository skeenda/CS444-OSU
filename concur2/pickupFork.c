#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

pthread_t philo_id[5];
sem_t forks[5];

struct philosopher{
	char* name;
	sem_t* left_fork;
	sem_t* right_fork;
};

void* preparePhilo(void *arg){
	int i;
	struct philosopher *philo;

	philo = malloc(sizeof(struct philosopher));
	for(i=0; i < sizeof(philo_id)/sizeof(philo_id[0]); i++){
		if(pthread_self() == philo_id[i]){
			printf("I am philosopher #%d\n", (i+1));
			switch(i){
				case 0:
					philo->name = "Plato";
					philo->left_fork = &forks[0];
					philo->right_fork = &forks[4];
			}
		}
	}
}

void* table(void *arg){
	printf("I think therefore I am\n");
}

int main(){
	int i;

	for(i=0; i < sizeof(forks)/sizeof(forks[0]); i++){
		sem_init(&forks[0], 0, 1);
	}

	for(i=0; i < sizeof(philo_id)/sizeof(philo_id[0]); i++){
		pthread_create(&philo_id[i], NULL, &preparePhilo, NULL);
	}
	
	for(i=0; i < sizeof(philo_id)/sizeof(philo_id[0]); i++){
		pthread_join(philo_id[i], NULL);
	}
}

