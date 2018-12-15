#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NO_OF_THREAD 2

int MAX_SIZE = 40;
int arr[40];

int bothEnded = 0;

void* search(void* para);
void arrayRandom();

int main() {

	arrayRandom();
	printf("Array(having size %d): ", MAX_SIZE);
	for(int i = 0; i < MAX_SIZE; i++) {
		printf("%d ", arr[i]);
	}

	int searchValue = 0;
	printf("\nWhat do you want to search: ");
	scanf("%d", &searchValue);

	int chunk = MAX_SIZE/2;

	pthread_t pids[NO_OF_THREAD];

	int* parameters1[4];
	parameters1[0] = &searchValue;
	int start1 = 0;
	parameters1[1] = &start1;
	parameters1[2] = &chunk;
	parameters1[3] = malloc(sizeof(int*));
	*parameters1[3] = -1;

	int* parameters2[4];
	parameters2[0] = &searchValue;
	int start2 = MAX_SIZE/2;
	parameters2[1] = &start2;
	parameters2[2] = &MAX_SIZE;
	parameters2[3] = malloc(sizeof(int*));
	*parameters2[3] = -1;

	int temp;

	temp = pthread_create(&pids[0], NULL, search, (void*) parameters1);
	if(temp != 0)
		printf("Error while creating thread 1\n");
	
	temp = pthread_create(&pids[1], NULL, search, (void*) parameters2);
	if(temp != 0)
		printf("Error while creating thread 2\n");

	while(1) 
	{
		if(*parameters1[3] != -1 && bothEnded == 1)
		{
			temp = pthread_cancel(pids[1]);
			if(temp != 0) {
				printf("ERROR: cancel command was not issued to thread 2 ");
			}
			break;
		}
		else if(*parameters2[3] != -1 && bothEnded == 1)
		{
			temp = pthread_cancel(pids[0]);
			if(temp != 0) {
				printf("ERROR: cancel command was not issued to thread 1 ");
			}
			break;
		}
		else if(bothEnded == 2)
		{
			printf("Not Found\n");
			break;
		}
	}


	pthread_join(pids[0], NULL);
	pthread_join(pids[1], NULL);

	free(parameters1[3]);
	free(parameters2[3]);

	pthread_exit(NULL);
	return 0;
}

void arrayRandom()
{
	for(int i = 0; i < MAX_SIZE; i++)
	{
		arr[i] = (rand() % 100) + 1;
	}
}

void* search(void* para) {

	int **parameter = para;
	int* searchedValue = parameter[0];	
	int* start = parameter[1];
	int* size = parameter[2];

	int temp = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
   	if (temp != 0)
       printf("ERROR: pthread_setcancelstate");

	for(int i = *start; i < *size; i++)
	{
		pthread_testcancel();
		if(arr[i] == *searchedValue) {
			*parameter[3] = i;
			printf("Found At Index: %d\n", i);
			bothEnded++;
			pthread_exit(NULL);
		}
	}

	bothEnded++;
	pthread_exit(NULL);
}