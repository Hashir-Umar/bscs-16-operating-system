#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t semaphoreLeft;
sem_t semaphoreRight;
pthread_rwlock_t lock;
pthread_mutex_t lockforcount;
void* leftCar(void* parameter);
void* rightCar(void* parameter);
void waitWhileCrossing(int time);

int rightcarcount = 0;

int main(int argc, char* argv[])
{
	const int noLeftCarThreads = 50;
	const int noRightCarThreads = 20;

	pthread_t leftCarThreads[noLeftCarThreads];
	pthread_t rightCarThreads[noRightCarThreads];


	if(sem_init(&semaphoreLeft, 0, 3)) {
		printf("Error while initializing samaphoreleft\n");
		return 0;
	}

	if(sem_init(&semaphoreRight, 0, 3)) {
		printf("Error while initializing samaphoreright\n");
		return 0;
	}
	
	if(pthread_rwlock_init(&lock, NULL)) {
		printf("Error while initializing rwlock lock \n");
		return 0;
	}


	if(pthread_mutex_init(&lockforcount, NULL)) {
		printf("Error while initializing mutex \n");
		return 0;
	}

	//creating threads
	for(int i = 0; i < noLeftCarThreads; i++) {
		if(pthread_create(&leftCarThreads[i], NULL, leftCar, NULL)) {
			printf("Error while creating left car thread %d\n", i + 1);
			return 0;
		}
	}

	for(int i = 0; i < noRightCarThreads; i++) 
	{
		if(pthread_create(&rightCarThreads[i], NULL, rightCar, NULL)) {
			printf("Error while creating right car thread \n");
			return 0;
		}
	}

	//waiting for all threads to complete their tasks
	for(int i = 0; i < noLeftCarThreads; i++) 
	{
		if(i < 4 && pthread_join(leftCarThreads[i], NULL)) {
			printf("Error while joining left car thread %d\n", i + 1);
			return 0;
		}
	}
	
	for(int i = 0; i < noRightCarThreads; i++) 
	{
		if(pthread_join(rightCarThreads[i], NULL)) {
			printf("Error while joining right car thread %d\n", i + 1);
			return 0;
		}
	}

	pthread_rwlock_destroy(&lock);
	pthread_mutex_destroy(&lockforcount);

	sem_destroy(&semaphoreLeft);
	sem_destroy(&semaphoreRight);

	return 0;
}

void* leftCar(void* parameter)
{
	pthread_rwlock_rdlock(&lock);
	sem_wait(&semaphoreLeft);
	printf("left car crossing\n");
	waitWhileCrossing(1);
	printf("left car crossed\n");
	sem_post(&semaphoreLeft);
	pthread_rwlock_unlock(&lock);
	return NULL;
}

void* rightCar(void* parameter)
{
	pthread_rwlock_rdlock(&lock);

	sem_wait(&semaphoreRight);
	printf("right car crossing\n");
	waitWhileCrossing(1);
	printf("right car crossed\n");
	sem_post(&semaphoreRight);
	pthread_rwlock_unlock(&lock);
	return NULL;
}

void waitWhileCrossing(int time)
{
	sleep(time);
}