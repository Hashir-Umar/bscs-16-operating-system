#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NO_OF_THREAD 4

int MAX_SIZE = 20;
int arr1[20];
int arr2[20];
int globleCheckingArray[40];
int sizeGloble = 0;

void arrayRandom(int arr[]);
void* findCommon(void* para);

int main() {

	pthread_t pids[NO_OF_THREAD];

	arrayRandom(arr1);
	arrayRandom(arr2);

	printf("Array 1 (having size %d): ", MAX_SIZE);
	for(int i = 0; i < MAX_SIZE; i++) {
		printf("%d ", arr1[i]);
	}

	printf("\nArray 2 (having size %d): ", MAX_SIZE);
	for(int i = 0; i < MAX_SIZE; i++) {
		printf("%d ", arr2[i]);
	}

	int start = 0;
	int chunk = MAX_SIZE/2;

	int* parameters1[4];
	parameters1[0] = &start;	//array 1 starting index
	parameters1[1] = &chunk;	//array 1 ending index
	parameters1[2] = &start;	//array 2 starting index
	parameters1[3] = &chunk;	///array 2 ending index

	int* parameters2[4];
	parameters2[0] = &chunk;	//array 1 starting index
	parameters2[1] = &MAX_SIZE;	//array 1 ending index
	parameters2[2] = &start;	//array 2 starting index
	parameters2[3] = &chunk;	///array 2 ending index

	int* parameters3[4];
	parameters3[0] = &start;	//array 1 starting index
	parameters3[1] = &chunk;	//array 1 ending index
	parameters3[2] = &chunk;	//array 2 starting index
	parameters3[3] = &MAX_SIZE;	///array 2 ending index

	int* parameters4[4];
	parameters4[0] = &chunk;	//array 1 starting index
	parameters4[1] = &MAX_SIZE;	//array 1 ending index
	parameters4[2] = &chunk;	//array 2 starting index
	parameters4[3] = &MAX_SIZE;	///array 2 ending index

	int temp;
	temp = pthread_create(&pids[0], NULL, findCommon, (void*) parameters1);
	if(temp != 0)
		printf("Error while creating thread 1\n");

	temp = pthread_create(&pids[1], NULL, findCommon, (void*) parameters2);
	if(temp != 0)
		printf("Error while creating thread 2\n");

	temp = pthread_create(&pids[2], NULL, findCommon, (void*) parameters3);
	if(temp != 0)
		printf("Error while creating thread 3\n");

	temp = pthread_create(&pids[3], NULL, findCommon, (void*) parameters4);
	if(temp != 0)
		printf("Error while creating thread 4\n");

	
	pthread_join(pids[0], NULL);
	pthread_join(pids[1], NULL);
	pthread_join(pids[2], NULL);
	pthread_join(pids[3], NULL);

	if(sizeGloble != 0) {
		printf("\nCommon Elements: ");
		for(int i = 0; i < sizeGloble; i++)
			printf("%d ", globleCheckingArray[i]);
	}
	else
		printf("\nNo Common Element were found\n");

	printf("\n");
	
	pthread_exit(NULL);
	return 0;
}

void* findCommon(void* para) {

	int **parameter = para;
	int* start1 = parameter[0];
	int* end1 = parameter[1];
	int* start2 = parameter[2];
	int* end2 = parameter[3];

	for(int i = *start1; i < *end1; i++) {
		for(int j = *start2; j < *end2; j++) {
			if(arr1[i] == arr2[j]) {
				globleCheckingArray[sizeGloble] = arr1[i]; 
				sizeGloble++;
			}
		}
	}

	pthread_exit(NULL);
}

void arrayRandom(int arr[])
{
	for(int i = 0; i < MAX_SIZE; i++) {
		arr[i] = (rand() % 100) + 1;
	}
}
