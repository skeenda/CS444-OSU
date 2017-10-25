#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <errno.h>

pthread_t philo_id[5];
sem_t forks[5];

struct philosopher{
	char* name;
	int left_fork;
	int right_fork;
};

void think(struct philosopher *philo){
	int wait_time;

	wait_time = rand() % 20 + 1;

	printf("%s is waiting for %d seconds\n", philo->name, wait_time);
	sleep(wait_time);
}

void get_forks(struct philosopher *philo){
	int sem_value;
	int got_forks = 1;

	while(got_forks){
		sem_getvalue(&forks[philo->left_fork], &sem_value);
		if(sem_value == 1){
			sem_wait(&forks[philo->left_fork]);

			sem_getvalue(&forks[philo->right_fork], &sem_value);
			if(sem_value == 1){
				sem_wait(&forks[philo->right_fork]);
				got_forks = 0;
			}
			else{
				sem_post(&forks[philo->left_fork]);
			}
		}
	}

	printf("%s has picked up forks %d and %d\n", philo->name, philo->left_fork, philo->right_fork);
}

void eat(struct philosopher *philo){
	int wait_time;

	wait_time = rand() % 8 + 2;

	printf("%s is eating for %d seconds\n", philo->name, wait_time);
	sleep(wait_time);
}

void put_forks(struct philosopher *philo){
	sem_post(&forks[philo->left_fork]);
	sem_post(&forks[philo->right_fork]);

	printf("%s put down forks %d and %d\n", philo->name, philo->left_fork, philo->right_fork);
}

void table(struct philosopher *philo){
	while(1){
		think(philo);
		get_forks(philo);
		eat(philo);
		put_forks(philo);
	}
}

void* preparePhilo(void *arg){
	int i;
	struct philosopher *philo;

	philo = malloc(sizeof(struct philosopher));
	for(i=0; i < sizeof(philo_id)/sizeof(philo_id[0]); i++){
		if(pthread_self() == philo_id[i]){
			switch(i){
				case 0:
					philo->name = "Plato";
					philo->left_fork = 0;
					philo->right_fork = 4;
					break;
				case 1:
					philo->name = "Socrates";
					philo->left_fork = 1;
					philo->right_fork = 0;
					break;
				case 2:
					philo->name = "Marx";
					philo->left_fork = 2;
					philo->right_fork = 1;
					break;
				case 3:
					philo->name = "Nietzsche";
					philo->left_fork = 3;
					philo->right_fork = 2;
					break;
				case 4:
					philo->name = "Machiavelli";
					philo->left_fork = 4;
					philo->right_fork = 3;
					break;
				default:
					printf("An error occurred.\n");
			}
		}
	}

	table(philo);
}

int main(){
	int i;

	srand(time(NULL));	/* Init random generator */

	for(i=0; i < sizeof(forks)/sizeof(forks[0]); i++){
		sem_init(&forks[i], 0, 1);
	}

	for(i=0; i < sizeof(philo_id)/sizeof(philo_id[0]); i++){
		pthread_create(&philo_id[i], NULL, &preparePhilo, NULL);
	}
	
	for(i=0; i < sizeof(philo_id)/sizeof(philo_id[0]); i++){
		pthread_join(philo_id[i], NULL);
	}
}

