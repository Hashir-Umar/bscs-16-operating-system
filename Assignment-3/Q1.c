#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMMBER_OF_ROWS 9
#define NUMMBER_OF_COLS 9
#define NUMBER_OF_THREADS 27    //9 threads for 9 blocks, 9 threads for 9 rows and 9 threads for 9 columns 

//valid solution
int box[NUMMBER_OF_ROWS][NUMMBER_OF_COLS] = 
{
    {8, 3, 5, 4, 1, 6, 9, 2, 7},
    {2, 9, 6, 8, 5, 7, 4, 3, 1},
    {4, 1, 7, 2, 9, 3, 6, 5, 8},
    {5, 6, 9, 1, 3, 4, 7, 8, 2},
    {1, 2, 3, 6, 7, 8, 5, 4, 9},
    {7, 4, 8, 5, 2, 9, 1, 6, 3},
    {6, 5, 2, 7, 8, 1, 3, 9, 4},
    {9, 8, 1, 3, 4, 5, 2, 7, 6},
    {3, 7, 4, 9, 6, 2, 8, 1, 5}
};

struct Location {
    int row, col;
};

void* rowCheck(void* parameter);
void* columnCheck(void* parameter);
void* blockCheck(void* parameter);
void showResult(long values[], int errorFlag);

int main() 
{
    pthread_t tid[NUMBER_OF_THREADS];
    long returnedValueFromEachThread[NUMBER_OF_THREADS];

    struct Location *location[NUMBER_OF_THREADS];
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        location[i] = (struct Location *) malloc(sizeof(*location));

    //initializing block locations
    int j = 0;
    for(int r = 0; r < 9; r+=3) {
        for(int c = 0; c < 3; c++) {
            location[j]->row = r;
            location[j++]->col = c*3;
        }
    }

    //initializing row locations
    for(int r = 0; r < NUMMBER_OF_ROWS; r++) {
        location[j]->row = r;
        location[j++]->col = 0;
    }

    //initializing column locations
    for(int c = 0; c < NUMMBER_OF_COLS; c++) {
        location[j]->row = 0;
        location[j++]->col = c;
    }

    j = 1;  //using this variable for error handling
    //creating 1st 9 threads for 9 blocks, 2nd 9 threads for 9 rows and third 9 threads for 9 columns 
    for(int i = 0; i < NUMBER_OF_THREADS; i++) 
    {
        if((i >= 0 && i < 9) && pthread_create(&tid[i], NULL, blockCheck, location[i])) {   
            j = 0;
        }
        else if((i >= 9 && i < 18) && pthread_create(&tid[i], NULL, rowCheck, location[i])) {
            j = 0;
        } 
        else if((i >= 18) && pthread_create(&tid[i], NULL, columnCheck, location[i])) {
            j = 0;
        }

        if(j == 0) {
            printf("Error: Creating thread %d\n", i+1);
            return 0;
        }

    }

    for(int i = 0; i < NUMBER_OF_THREADS; i++) {
        if(pthread_join(tid[i], (void*) &returnedValueFromEachThread[i])) {
            printf("Error: Joining thread %d\n", i+1);
            return 0;
        }
    }    

    j = 1;  //this 1 will display invalid regions on console. Change this value to 0 so it will only tells you whether a solution is valid or not
    showResult(returnedValueFromEachThread, j);    
    
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        free(location[i]);
    }

    return 0;
}

void* blockCheck(void* parameter)
{
    int arr[9]; //array to save each index value to check whether a value is repeating in array or not (like hashmap data structure)
    int size = 0;

    for(int i = 0; i < 9; i++)
            arr[i] = 0;
    
    int passed_row = (int) ((struct Location*) parameter)->row;
    int passed_col = (int) ((struct Location*) parameter)->col; 

    for(int row = passed_row; row < passed_row + 3; row++) 
    {
        for(int col = passed_col; col < passed_col + 3; col++) 
        {
            //this block won't execute at first iteration
            if(size != 0)
            {
                //checking if current index's value present in above created array or not
                if(arr[box[row][col] - 1] != 0) {
                    //if value found in above created array then we can say that we found a duplicate value which is an invalid number  
                    return (void *) 0;
                }
            }

            size = 1;
            arr[box[row][col] - 1] = box[row][col];
        }
    }
    
    return (void *) 1;
}

void* rowCheck(void* parameter)
{
    int arr[NUMMBER_OF_ROWS]; //array to save each index value to check whether a value is repeating or not (like hashmap data structure)
    
    for(int i = 0; i < NUMMBER_OF_ROWS; i++)
            arr[i] = 0;

    int passed_row = (int) ((struct Location*) parameter)->row;
    int passed_col = (int) ((struct Location*) parameter)->col; //passed_col always remain 0 in row thread

    passed_col = 0; //assuring it to be 0 in each thread
    arr[box[passed_row][passed_col] - 1] = box[passed_row][passed_col];  //storing 1st index of box value in array

    for(int col = 1; col < NUMMBER_OF_COLS; col++) 
    {
        //checking if current index's value present in above created arr or not
        if(arr[box[passed_row][col] - 1] != 0) {
            //if value found in above created array then we can say that we found a duplicate value which is an invalid number
            return (void *) 0;
        }

        //if current index's value not found then it will be inserted to arr    
        arr[box[passed_row][col] - 1] = box[passed_row][col];
    }


    return (void *) 1;
}

void* columnCheck(void* parameter)
{
    int arr[NUMMBER_OF_COLS]; //array to save each index value to check whether a value is repeating in array or not (like hashmap data structure)
    
    for(int i = 0; i < NUMMBER_OF_COLS; i++)
        arr[i] = 0;

    int passed_row = (int) ((struct Location*) parameter)->row; //passed_row always remain 0 in column thread
    int passed_col = (int) ((struct Location*) parameter)->col; 

    passed_row = 0; //assuring it to be 0 in each thread
    arr[box[passed_row][passed_col] - 1] = box[passed_row][passed_col];  //storing 1st index of box value in array

    for(int row = 1; row < NUMMBER_OF_ROWS; row++) 
    {
        //checking if current index's value present in above created array or not
        if(arr[box[row][passed_col] - 1] != 0) {
            //if value found in above created array then we can say that we found a duplicate value which is an invalid number  
            return (void *) 0;
        }

        //if current index's value not found then it will be pushed back to arr
        arr[box[row][passed_col] - 1] = box[row][passed_col];
    }

    return (void *) 1;
}

void showResult(long values[], int errorFlag)
{
    if(errorFlag == 1)
    {
        int blockRegion = 1, rowRegion = 1, columnRegion = 1;

        for (int i = 0; i < NUMBER_OF_THREADS; i++) 
        {
            if(values[i] == 0) {

                //this if statement executes only when 1st invalid region found
                if(errorFlag == 1) {
                    printf("Sudoku puzzle is invalid.\n");
                    errorFlag = 0;
                }

                if(i >= 0 && i < 9 && blockRegion == 1) {
                    //Block number starts with 0, so 1st block named as block number 0
                    printf("\n\tInvalid regions of Blocks are: ");
                    blockRegion = 0;
                }
                else if(i >= 9 && i < 18 && rowRegion == 1) {    
                    //Row number starts with 0, so 1st row named as row number 0
                    printf("\n\tInvalid regions of Rows are: ");
                    rowRegion = 0;
                }
                else if(i >= 18 && columnRegion == 1) {
                    //Column number starts with 0, so 1st row named as row number 0
                    printf("\n\tInvalid regions of Columns are: ");
                    columnRegion = 0;
                }

                printf(" %d", i % 9);
            }
        }

        if(blockRegion && rowRegion && columnRegion)
            printf("Sudoku puzzle is valid.");

        printf("\n");
    }
    else
    {
        for(int i = 0; i < NUMBER_OF_THREADS; i++)
        {
            if(values[i] == 0) {
                printf("Sudoku puzzle is invalid.\n");
                return;
            }
        }

        printf("Sudoku puzzle is valid.\n");
    }
}