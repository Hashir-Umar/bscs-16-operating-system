#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define MAX_SIZE 1000000
#define NO_OF_THREADS 5

int arr[MAX_SIZE];
long sum = 0;
int secondMax = 0;

struct Range {
	int start, end;
};

void* chunkMaximum(void* startingIndex);

int main() {

	for(int i = 0; i < MAX_SIZE; i++) {
		arr[i] = (int)rand();
	}

	struct Range r[NO_OF_THREADS];

	r[0].start = 0;
	r[0].end = 200000;

	r[1].start = 200000;
	r[1].end = 400000;

	r[2].start = 400000;
	r[2].end = 600000;

	r[3].start = 600000;
	r[3].end = 800000;

	r[4].start = 800000;
	r[4].end = MAX_SIZE - 1;

	pthread_t pids[NO_OF_THREADS];

	for(int i = 0; i < NO_OF_THREADS; i++)
	{
		int rc = pthread_create(&pids[i], NULL, chunkMaximum, (void *) &r[i]);
		if(rc != 0) {
			printf("Error while creating Thread 1\n");
		}
	}

	void* localmaxArraymum[NO_OF_THREADS];
	for(int i = 0; i < NO_OF_THREADS; i++)
		pthread_join(pids[i], &localmaxArraymum[i]);

	for(int i = 0; i < NO_OF_THREADS; i++)
		printf("Thread %d Returns(Local Maximum): %d\n", i+1, *((int*)localmaxArraymum[i]));

	int maxArray[4];
	for(int i = 0; i < 4; i++)
		maxArray[i] = *((int*)localmaxArraymum[i]);

	int first = 0;
    for (int i = 0; i < 4; i++) 
    { 
        if (maxArray[i] > first) 
        { 
            secondMax = first; 
            first = maxArray[i]; 
        }
        else if (maxArray[i] > secondMax && maxArray[i] != first) 
            secondMax = maxArray[i]; 
    } 


	printf("\nSum: %ld\n", sum);
	printf("Second Maximum: %d\n", secondMax);

	pthread_exit(NULL);
	return 0;
}

void* chunkMaximum(void* startingIndex)
{

	int* localMax = (int*) malloc(sizeof(int*));
	*localMax = arr[0];

	struct Range *r = (struct Range *) startingIndex;
	for(int i = r->start; i < r->end; i++) 
	{
		if(arr[i] > *localMax)
			*localMax = arr[i];

		sum += arr[i];
	}

	pthread_exit((void*)localMax);
}
