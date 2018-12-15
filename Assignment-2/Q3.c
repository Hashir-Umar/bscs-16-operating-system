#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

char string[100];
char substring[100];
int stringSize, substringSize;
#define NO_OF_THREAD 0

void* stringCompare(void *parameter);

int main() {

	FILE *fileptr = fopen("strings.txt", "r");

	if(fileptr == NULL) {
		printf("Error while opening file\n");
	}
	else
	{
		fscanf(fileptr, "%s", string);
		fscanf(fileptr, "%s", substring);

		printf("String %s\n", string);
		printf("Substring %s\n", substring);

		//taking size of strings
		stringSize = strlen(string);
		substringSize = strlen(substring);

		//String size should be valid as required in question
		if(stringSize % substringSize == 0) {

			int noOfThreads = (int) (stringSize - 1);

			#undef NO_OF_THREAD
			#define NO_OF_THREAD noOfThreads

			pthread_t *pids = (pthread_t *) calloc(noOfThreads, sizeof(pthread_t));

			int temp;
			int *staringIndex = (int*) calloc(noOfThreads, sizeof(int));
			for (int i = 0; i < noOfThreads; i++) 
			{
				staringIndex[i] = i;
				temp = pthread_create(&pids[i], NULL, stringCompare, (void*) &staringIndex[i]);

				if(temp)
					printf("Error while creating thread %d\n", i+1);
			}

			//variable to handle local matching
			int *value = (int*) calloc(noOfThreads, sizeof(int));
			for (int i = 0; i < noOfThreads; i++) 
				pthread_join(pids[i], (void *)&value[i]);

			//variable for globle matching
			int globleSum = 0;
			for (int i = 0; i < noOfThreads; i++) { 
				globleSum += value[i];
			}

			printf("\nTotal Substring Count: %d\n", globleSum);

			free(value);
			free(pids);
		} 
		else {
			printf("Invalid String sizes\n");
		}

		fclose(fileptr);
	}

	pthread_exit(NULL);
	return 0;
}

void* stringCompare(void *parameter)
{
	int index = 0;
	int *start = (int *)parameter;
	for(int i = *start; i < substringSize + (*start); i++) {
		if(string[i] != substring[index++])
			pthread_exit((void*) 0);
	}
	pthread_exit((void*) 1);
}