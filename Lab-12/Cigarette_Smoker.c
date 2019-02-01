#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

void *agent(void *parameter);
void *smoker(void *parameter);

typedef enum {PAPER, MATCH, TOBECCO} RESOURCES;

sem_t agentSemaphore;
sem_t smokerSemaphore[3];

struct smokerThreadInput
{
	int number;
	RESOURCES res;
};

int main()
{
	struct smokerThreadInput input[3];
	pthread_t smokerThread[3];
	pthread_t agentThread;

	for(int i = 0; i < 3; i++)
	{
		if(sem_init(&smokerSemaphore[i], 0, 1))
		{ 
			printf("Error while initializing smoker semaphore %d\n", i + 1);
			return 0;
		}
	}

	if(sem_init(&agentSemaphore, 0, 1))
	{ 
		printf("Error while initializing agent semaphore\n");
		return 0;
	}

	for(int i = 0; i < 3; i++)
	{
		input[i].number = i;
		input[i].res = i;
		if(pthread_create(&smokerThread[i], NULL, smoker, (void *) &input[i]))
		{ 
			printf("Error while creating smoker thread %d\n", i + 1);
			return 0;
		}
	}

	if(pthread_create(&agentThread, NULL, agent, NULL))
	{ 
		printf("Error while creating agent thread\n");
		return 0;
	}

	for(int i = 0; i < 3; i++)
	{
		if(pthread_join(smokerThread[i], NULL))
		{ 
			printf("Error while joining smoker thread %d\n", i + 1);
			return 0;
		}
	}

	if(pthread_join(agentThread, NULL))
	{ 
		printf("Error while creating agent thread\n");
		return 0;
	}

	for(int i = 0; i < 3; i++)
	{
		if(sem_destroy(&agentSemaphore))
		{ 
			printf("Error while destroying smoker semaphore %d\n", i + 1);
			return 0;
		}
	}

	return 0;
}

void *agent(void *parameter)
{
	int res1;
	int res2;

	while(1)
	{
		sem_wait(&agentSemaphore);
		res1 = rand() % 3;
		res2 = rand() % 3;
		//this while loop ensures that both res values are different
		while(res1 == res2)
			res2 = rand() % 3;

		if(res1 == PAPER && res2 == MATCH)
			sem_post(&smokerSemaphore[TOBECCO]);
		else if(res1 == TOBECCO && res2 == PAPER)
			sem_post(&smokerSemaphore[MATCH]);
		else if(res1 == TOBECCO && res2 == MATCH)
			sem_post(&smokerSemaphore[PAPER]);

		//supplying items
		sleep(1);	
	}

	return NULL;
}

void *smoker(void *parameter)
{
	struct smokerThreadInput input = *((struct smokerThreadInput*) parameter);

	while(1)
	{
		if(input.res == MATCH) 
		{
			//this smoker thread has MATCH initially
			printf("\nSmoker %d needed TOBECCO and PAPER", input.number);
			sem_wait(&smokerSemaphore[MATCH]);
			printf("\nSmoker %d Making a Cigarette", input.number);	
		}
		
		if(input.res == TOBECCO) 
		{
			//this smoker thread has TOBECCO initially
			printf("\nSmoker %d needed MATCH and PAPER", input.number);
			sem_wait(&smokerSemaphore[TOBECCO]);
			printf("\nSmoker %d Making a Cigarette", input.number);

		}
		
		if(input.res == PAPER) 
		{
			//this smoker thread has PAPER initially
			printf("\nSmoker %d needed TOBECCO and MATCH", input.number);
			sem_wait(&smokerSemaphore[PAPER]);
			printf("\nSmoker %d Making a Cigarette", input.number);	
		}

		printf("\nSmoker %d smoking", input.number);
		printf("\nSmoker %d has finished smoking", input.number);
		sem_post(&agentSemaphore);
	}

	return NULL;
}