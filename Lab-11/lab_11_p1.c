#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>


sem_t semaphore;
pthread_rwlock_t lock;
void* car(void* parameter);
void* truck(void* parameter);
void waitWhileCrossing(int time);

int main(int argc, char* argv[])
{
	const int noTruckThreads = 10;
	const int noCarThreads = 100;

	pthread_t carThreads[noCarThreads];
	pthread_t truckThreads[noTruckThreads];


	if(sem_init(&semaphore, 0, 3)) {
		printf("Error while initializing samaphore\n");
		return 0;
	}
	
	if(pthread_rwlock_init(&lock, NULL)) {
		printf("Error while creating thread \n");
		return 0;
	}
	//creating threads
	for(int i = 0; i < noCarThreads; i++) {
		if(pthread_create(&carThreads[i], NULL, car, NULL)) {
			printf("Error while creating thread %d\n", i + 1);
			return 0;
		}
	}

	for(int i = 0; i < noTruckThreads; i++) 
	{
		if(pthread_create(&truckThreads[i], NULL, truck, NULL)) {
			printf("Error while creating truck thread \n");
			return 0;
		}
	}

	//waiting for all threads to complete their tasks
	for(int i = 0; i < noCarThreads; i++) 
	{
		if(i < 4 && pthread_join(carThreads[i], NULL)) {
			printf("Error while creating thread %d\n", i + 1);
			return 0;
		}
	}
	
	for(int i = 0; i < noTruckThreads; i++) 
	{
		if(pthread_join(truckThreads[i], NULL)) {
			printf("Error while creating thread %d\n", i + 1);
			return 0;
		}
	}

	pthread_rwlock_destroy(&lock);

	sem_destroy(&semaphore);

	return 0;
}

void* car(void* parameter)
{
	pthread_rwlock_rdlock(&lock);
	sem_wait(&semaphore);
	printf("car crossing\n");
	waitWhileCrossing(0);
	printf("car crossed\n");
	sem_post(&semaphore);
	pthread_rwlock_unlock(&lock);
	return NULL;
}

void* truck(void* parameter)
{
	pthread_rwlock_wrlock(&lock);
	printf("truck crossing\n");
	waitWhileCrossing(0);
	printf("truck crossed\n");
	pthread_rwlock_unlock(&lock);
	return NULL;
}

void waitWhileCrossing(int time)
{
	sleep(time);
}