#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <errno.h>

pthread_t philo_id[5]; /* holds thread ids for philosophers */
sem_t forks[5]; /* semaphores representing the forks */

/* struct to hold philospher details */
struct philosopher{
	char* name;
	int left_fork;
	int right_fork;
};

/* puts philosophers to sleep so they have a random amount of time to think */
void think(struct philosopher *philo){
	int wait_time;

	wait_time = rand() % 20 + 1;

	printf("%s is waiting for %d seconds\n", philo->name, wait_time);
	sleep(wait_time);
}

/* teaches philosophers how to pickup forks
 * teaches them how to share because they can't take other philosophers forks
 */
void get_forks(struct philosopher *philo){
	int sem_value;
	int got_forks = 1;

	while(got_forks){
		/* check if left fork is picked up
		   continue looping if it is */
		sem_getvalue(&forks[philo->left_fork], &sem_value);
		if(sem_value == 1){
			/* fork is available so pick it up */
			sem_wait(&forks[philo->left_fork]);

			/*check if right fork is picked up */
			sem_getvalue(&forks[philo->right_fork], &sem_value);
			if(sem_value == 1){
				/* fork is available so pick it up */
				sem_wait(&forks[philo->right_fork]);
				/* both forks are up stop looping */
				got_forks = 0;
			}
			else{
				/* right fork can't be picked up
 				   must drop left fork to free it up */
				sem_post(&forks[philo->left_fork]);
			}
		}
	}

	printf("%s has picked up forks %d and %d\n", philo->name, philo->left_fork, philo->right_fork);
}

/* teaches philosophers to eat by giving them a random amount of time to eat */
void eat(struct philosopher *philo){
	int wait_time;

	wait_time = rand() % 8 + 2;

	printf("%s is eating for %d seconds\n", philo->name, wait_time);
	sleep(wait_time);
}

/* teaches philosophers how to put down forks by incrementing semaphore */
void put_forks(struct philosopher *philo){
	sem_post(&forks[philo->left_fork]);
	sem_post(&forks[philo->right_fork]);

	printf("%s put down forks %d and %d\n", philo->name, philo->left_fork, philo->right_fork);
}

/* table the philosophers are sitting around
 * controls philosophers behavior
 */
void table(struct philosopher *philo){
	while(1){
		think(philo);
		get_forks(philo);
		eat(philo);
		put_forks(philo);
	}
}

/* setup each philosophers struct
 * their thread id is used to determine where they sit at the table
 */
void* preparePhilo(void *arg){
	int i;
	struct philosopher *philo;

	philo = malloc(sizeof(struct philosopher));
	/*figure out which thread is running this code*/
	for(i=0; i < sizeof(philo_id)/sizeof(philo_id[0]); i++){
		if(pthread_self() == philo_id[i]){
			/*each philosopher has a unique name 
 			so they are setup separately*/
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

	/* philosopher is ready to sit at the table*/
	table(philo);
}

int main(){
	int i;

	srand(time(NULL));	/* Init random generator */

	/* initialize semaphore forks */
	for(i=0; i < sizeof(forks)/sizeof(forks[0]); i++){
		sem_init(&forks[i], 0, 1);
	}

	/* spawn philosopher threads */
	for(i=0; i < sizeof(philo_id)/sizeof(philo_id[0]); i++){
		pthread_create(&philo_id[i], NULL, &preparePhilo, NULL);
	}
	
	/* wait for threads to finish which is never */
	for(i=0; i < sizeof(philo_id)/sizeof(philo_id[0]); i++){
		pthread_join(philo_id[i], NULL);
	}
}

