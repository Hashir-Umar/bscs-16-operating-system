#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 20
#define NUMBER_OF_THREADS 3

struct Chunk
{
    int start, end;   
};

int arr1[ARRAY_SIZE];
int arr2[ARRAY_SIZE];

void initialize(int arr[]);
void display(int arr[]);
void* sort(void *parameter);
void* merge(void *parameter);

int main() 
{   
    pthread_t tid[NUMBER_OF_THREADS];

    //chunk structure holding the starting and ending indecies of the array 
    struct Chunk chunk[NUMBER_OF_THREADS];

    //starting and ending indecies of first half 
    chunk[0].start = 0;
    chunk[0].end = ARRAY_SIZE/2;

    //starting and ending indecies of second half 
    chunk[1].start = ARRAY_SIZE/2;
    chunk[1].end = ARRAY_SIZE;
    
    initialize(arr1);

    printf("\nOrignal Array: ");
    display(arr1);

    for(int i = 0; i < 2; i++)
    {
        if(pthread_create(&tid[i], NULL, sort, (void *) &chunk[i])) {
            printf("Error while creating thread %d\n", i+1);
            return 0;
        }
    }

    for(int i = 0; i < 2; i++)
    {
        if(pthread_join(tid[i], NULL)) {
            printf("Error while joining thread %d\n", i+1);
            return 0;
        }
    }

    printf("\nArray after sorting each half: ");
    display(arr1);

    ///////////////////////    Merging Thread    ///////////////////////
    ///////////////////////////////////////////////////////////////////

    chunk[2].start = 0;     //strting index of first half of the array
    chunk[2].end = ARRAY_SIZE/2;      //strting index of Second half of the array
   
    if(pthread_create(&tid[2], NULL, merge, (void *) &chunk[2])) {
        printf("Error while creating Merge thread\n");
        return 0;
    }

    if(pthread_join(tid[2], NULL)) {
        printf("Error while joining Merge thread\n");
        return 0;
    }

    printf("\nSecond Array after merging: ");
    display(arr2);

    ///////////////////////////////////////////////////////////////////

    return 0;
}

//Bubble sort algo
void* sort(void *parameter)
{
    int flag = 1;

    int startingIndex = (int)((struct Chunk *) parameter) -> start;
    int endingIndex = (int)((struct Chunk *) parameter) -> end;

    while(flag)
    {
        flag = 0;
        for(int i = startingIndex; i < endingIndex - 1; i++)
        {
            if(arr1[i] > arr1[i + 1])
            {
                int temp = arr1[i];
                arr1[i] = arr1[i + 1];
                arr1[i + 1] = temp;
                flag = 1;
            }
        }
    }
    return NULL;
}

void* merge(void *parameter)
{
    //starting index of first half of the array
    int index1 = (int)((struct Chunk *) parameter) -> start;

    //starting index of second half of the array
    int index2 = (int)((struct Chunk *) parameter) -> end;

    int i = 0;
    while(i < ARRAY_SIZE)
    {
        if(arr1[index1] < arr1[index2]) 
        {
            arr2[i++] = arr1[index1++];
            if(index1 >= ARRAY_SIZE / 2)
            {
                //this loop will transfer remaining values to newly created array, 
                //as we know remaining values are in sorted order and there is no other values in 2nd half of the array  
                for(int j = index2; j < ARRAY_SIZE; j++) {
                    arr2[i++] = arr1[j];
                }
                break;
            }
        }
        else 
        {
            arr2[i++] = arr1[index2++];
            if(index2 >= ARRAY_SIZE)
            {
                //this loop will transfer remaining values to newly created array, 
                //as we know remaining values are in sorted order and there is no other values in 1st half of the array  
                for(int j = index1; j < ARRAY_SIZE / 2; j++) {
                    arr2[i++] = arr1[j];
                }
                break;
            }
        }
    }

    return NULL;
}

void initialize(int arr[])
{
    for(int i = 0; i < ARRAY_SIZE; i++)
        arr[i] = rand() % 100 + 1;   
}

void display(int arr[])
{
    printf("\n\t");
    for(int i = 0; i < ARRAY_SIZE; i++)
        printf("%d ", arr[i]);
    printf("\n");
}