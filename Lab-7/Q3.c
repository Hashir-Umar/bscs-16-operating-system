#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

void *estimatePi(void *para);
int main(int argc, char* argv[]) {

	if(argc != 3)
		printf("Invalid Arguments\n");
	else
	{
		int noOfThreads = atoi(argv[1]);
		long long noOfTerms = atoll(argv[2]);
		
		if(noOfTerms > 100000) 
		{
			if(noOfTerms % noOfThreads == 0)
			{
				long long chunk =  (long long) (noOfTerms / noOfThreads);
				pthread_t* pids = (pthread_t*) calloc(noOfThreads, sizeof(pthread_t));

				int** parameter = (int**) calloc(noOfThreads, sizeof(int*));
				double** result = (double**) calloc(noOfThreads, sizeof(double));

				for(int i = 0; i < noOfThreads; i++)
				{
					parameter[i] = (int*) calloc(2, sizeof(int));
					parameter[i][0] = (chunk * i);
					parameter[i][1] = (chunk * i) + chunk;

					int temp;
					temp = pthread_create(&pids[i], NULL, estimatePi, (void *) &parameter[i]);
					if(temp) {
						printf("Error while creating thread %d\n", i + 1);
					}

					temp = pthread_join(pids[i], (void *) &result[i]);
					if(temp) {
						printf("Error while joining thread %d\n", i + 1);
					}
				}

				double total = 0;
				for(int i = 0; i < noOfThreads; i++) {
					total += result[i][0];
				}

				total *= 4;

				printf("Final value of Pi using %lld terms = %f\n", noOfTerms, total);

				for(int i = 0; i < noOfThreads; i++)
					free(result[i]);
			}
			else {
				printf("no Of terms Should be evenly divisible by the number of No of threads\n");
			}
		}
		else {
			printf("No of terms should be greater then 100000\n");
		}
	}

	pthread_exit(NULL);
	return 0;
}

void *estimatePi(void *para)
{
	int **parameter = (int**) para;
	int start = parameter[0][0];
	int end = parameter[0][1];
	double* result = (double*) malloc(sizeof(double*));
	*result = 0;

	for(int i = start; i < end; i++)
	{
		//assuming x = 1 So x to the power (2n + 1) would be negligible
		if(i % 2 == 0)
			*result += (1) / ((2 * i) + 1.0);
		else
			*result += (-1) / ((2 * i) + 1.0);
	}

	free(parameter[0]);
	pthread_exit((void*)result);
}