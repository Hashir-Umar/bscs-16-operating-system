#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define MAX_THREAD 10

int m1_rows = 4;
int m1_cols = 5;

int m2_rows = 5;
int m2_cols = 4;
int **matrix1, **matrix2;

int** arraryInitializer(int row, int col, int userInput);
void display(int **matrix, int row, int col);
void* calculate(void *parameter);

int main() {

	int userInput = 0;	//replace it by 1 for user input

	if(userInput == 1) {
		printf("Enter how many rows and colmns you want to Enter in matrix 1:\n");
		scanf("%d",&m1_rows);
		scanf("%d",&m1_cols);

		printf("\nEnter how many rows and colmns you want to Enter in matrix 2:\n");
		scanf("%d",&m2_rows);
		scanf("%d",&m2_cols);

	}

	if (m1_cols != m2_rows) {
		printf("columns of matrix 1 should be equal to rows of matrix 2.\n");
		return 0;
	}

	matrix1 = arraryInitializer(m1_rows, m1_cols, userInput);	
	printf("\nMatrix 1:\n");
	display(matrix1, m1_rows, m1_cols);
	
	matrix2 = arraryInitializer(m2_rows, m2_cols, userInput);	
	printf("\nMatrix 2:\n");
	display(matrix2, m2_rows, m2_cols);


	pthread_t pids[MAX_THREAD];

	for(int i = 0; i < m1_rows; i++)
		if(pthread_create(&pids[i], NULL, calculate, (void *) matrix1[i]))
			printf("Error while creating thread %d\n", i+1);

	int **result;
	result = (int**)calloc(m1_rows, sizeof(int*));

	for(int i = 0; i < m1_rows; i++)
		pthread_join(pids[i], (void*) &result[i]);

	printf("\nResult Matrix:\n");
	display(result, m1_rows, m2_cols);

	pthread_exit(NULL);
	return 0;
}

int** arraryInitializer(int row, int col, int userInput)
{
	int **matrix = (int**)calloc(row, sizeof(int*));

	for(int i = 0; i < row; i++)
		matrix[i] =  (int*) calloc(col, sizeof(int));

	for(int i = 0; i < row; i++) {
		
		if(userInput == 1) {
			printf("\nRow %d:-\n", i);
		}

		for(int j = 0; j < col; j++) {

			if(userInput == 1) {
				printf("\tColumn %d: ", j);
	        	scanf("%d",&matrix[i][j]);
			}
			else
				matrix[i][j] = (i + 1) * j + 1;
		}
	}

	return matrix;

}

void* calculate(void *parameter)
{
	int *sum = (int*) calloc(m1_rows, sizeof(int));
	int* arr = (int*)parameter;
	
	int k = 0;
	for(int i = 0; i < m1_cols; i++) {
		for(int j = 0; j < m2_rows; j++) {
			sum[k] += arr[j] * matrix2[j][i];
		}
		k++;
	}
	
	return (void *) sum;
}

void display(int **matrix, int row, int col)
{
	for(int i = 0; i < row; i++) {
		for(int j = 0; j < col; j++) {
			printf("\t%d ", matrix[i][j]);
		}
		printf("\n");
	}
}