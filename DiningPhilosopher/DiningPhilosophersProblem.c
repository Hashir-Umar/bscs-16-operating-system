#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define SLEEP_AMOUNT 30
#define PHILOSOPHERS 5

int philosophers_number[PHILOSOPHERS];
pthread_mutex_t philosophers_chopsticks_mutex[PHILOSOPHERS];
pthread_mutex_t meal_mutex;
int meals_eaten_by_each_philosopher[PHILOSOPHERS];
int TOTAL_MEALS = 20;	//default value

sem_t semaphore;

void* upEatDown(void* parameter);
int test_lock_and_eat_meal(int id);
void pick_chopsticks(int myId);
void put_chopsticks(int myId);
void eating(int myId);
void thinking(int myId);

int main(int argc, char* argv[])
{
	srand(time(NULL));

	if(argc != 2)
	{
		printf("USAGE: <binary name> <total meals should be equal or greater then 5>\n");
		return 0;
	}

	TOTAL_MEALS = atoi(argv[1]);

	//initializing locks
	for(int i = 0; i < PHILOSOPHERS; i++)
		pthread_mutex_init(&philosophers_chopsticks_mutex[i], NULL);

	pthread_t tid[PHILOSOPHERS];
	sem_init(&semaphore, 0, PHILOSOPHERS - 1);

	//creating threads
	for(int i = 0; i < PHILOSOPHERS; i++) {
		meals_eaten_by_each_philosopher[i] = 0;
		philosophers_number[i] = i;
		if(pthread_create(&tid[i], NULL, upEatDown, (void*) &philosophers_number[i])) {
			printf("Error while creating thread %d\n", i + 1);
			return 0;
		}
	}

	//waiting for all threads to complete their tasks
	for(int i = 0; i < PHILOSOPHERS; i++) {
		if(pthread_join(tid[i], NULL)) {
			printf("Error while creating thread %d\n", i + 1);
			return 0;
		}
	}

	for(int i = 0; i < PHILOSOPHERS; i++)
		printf("Philosopher %d ate %d meals\n", i, meals_eaten_by_each_philosopher[i]);

	//destroying locks
	for(int i = 0; i < PHILOSOPHERS; i++)
		pthread_mutex_destroy(&philosophers_chopsticks_mutex[i]);
	sem_destroy(&semaphore);

	return 0;
}

void* upEatDown(void* parameter)
{
	int myId = *((int*)parameter);
	
	while(1)
	{
		//it means all philosophers had eaten their meals
		int hungeryThreads = test_lock_and_eat_meal(myId);
		if(hungeryThreads == -1)
			return NULL;

		printf("Philosophers %d hungry\n", myId);

		sem_wait(&semaphore);
		pick_chopsticks(myId);

		eating(myId);

		put_chopsticks(myId);
		sem_post(&semaphore);

		thinking(myId);
		
	}

	pthread_exit(NULL); 
}

int test_lock_and_eat_meal(int id)
{
	pthread_mutex_lock(&meal_mutex);
	if(TOTAL_MEALS == 0)
	{
		pthread_mutex_unlock(&meal_mutex);
		return -1;
	}

	int hungry = 0;
	for(int i = 0; i < PHILOSOPHERS; i++)
	{
		if(i != id && meals_eaten_by_each_philosopher[i] == 0)
			hungry++;
	}

	if(hungry >= TOTAL_MEALS)
	{
		pthread_mutex_unlock(&meal_mutex);
		return -1;
	}
	
	TOTAL_MEALS--;	//eat his meal
	meals_eaten_by_each_philosopher[id]++;	//updating his meal count
	pthread_mutex_unlock(&meal_mutex);
	return 0;
}

void pick_chopsticks(int myId)
{
	pthread_mutex_lock(&philosophers_chopsticks_mutex[myId]);
	printf("Philosophers %d picks his left chopstick\n", myId);

	pthread_mutex_lock(&philosophers_chopsticks_mutex[(myId+1)%PHILOSOPHERS]);
	printf("Philosophers %d picks his right chopstick\n", myId);
}

void put_chopsticks(int myId)
{
	pthread_mutex_unlock(&philosophers_chopsticks_mutex[myId]);
	printf("Philosophers %d put down his left chopstick\n", myId);

	pthread_mutex_unlock(&philosophers_chopsticks_mutex[(myId+1)%PHILOSOPHERS]);
	printf("Philosophers %d put down his left chopstick\n", myId);
}

void eating(int myId)
{
	printf("Philosophers %d is eating\n", myId);
	sleep(rand() % SLEEP_AMOUNT);
}

void thinking(int myId)
{
	printf("Philosophers %d is eating\n", myId);
	sleep(rand() % SLEEP_AMOUNT);
}