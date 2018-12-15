#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
extern int errno;

void parseToArgv(char *argv[], char arr[], int *argc, int flag);
void changeDirectory(char path[], char *dir);
FILE* inputFromFile(char *argv[]); 
FILE* outputToFile(char *argv[]);
void pipeFuntion(char *argv[], int pos);
int getPipePosition(char *argv[]);

int main() 
{
	char line[100];
	char *argv[100];

	for (int i = 0; i < 100; i++)
		argv[i] = NULL;

	int argc = 0;
	int flag;
	char path[30] = "";

	getcwd(path, 100);
	
	while (1) 
	{
		flag = 0;
		printf("%s @MyBash > ", path);
	
		//Taking line as input 
		if (fgets(line, sizeof(line), stdin))
			line[strcspn(line, "\n")] = '\0';
		else
			break;

		//if user dont enter anything then it will not considered as input
		if(line[0] == '\0')
			continue;

		//this bash will exit when user enters exit
		if (strcmp(line, "exit") == 0) {
			break;
		}

		if(line[strlen(line) - 1] == '&') {
			flag = 1;
		}

		//paring line to arguments using space separation
		parseToArgv(argv, line, &argc, flag);
		int pipePosition = getPipePosition(argv);

		if (strcmp(argv[0], "cd") == 0) {
			changeDirectory(path, argv[1]);
		} 
		else if(pipePosition != 0) {
			pipeFuntion(argv, pipePosition);
			getcwd(path, 100);	
		}
		else 
		{
			if (fork() == 0) 
			{
				FILE *input = inputFromFile(argv);
				FILE *output = outputToFile(argv);

				if(input) 
					dup2(fileno(input), STDIN_FILENO);
				
				if(output)
					dup2(fileno(output), STDOUT_FILENO);

				//removing I/O redictors Symbols
				int i = 1;
				while(argv[i]) {
					if(strcmp(argv[i], "<") == 0) {
						printf("%s", argv[i]);

						argv[i] = NULL;	
						break;
					}
					i++;
				}
				////////////////////////////////

				if(execvp(argv[0], argv) == -1)
					printf("%s\n", strerror(errno));
			}
			else
			{
				if(flag == 0) {
					int status=0;
					wait(&status);
				}

				getcwd(path, 100);
			}
		}
	}

	for (int i = 0; i < argc; i++) {
		free(argv[i]);
		argv[i] = NULL;
	}

	return 0;
}

void changeDirectory(char path[], char *dir) 
{
	if(dir == NULL || strcmp(dir, "\0") == 0 || strcmp(dir, ".") == 0 || strcmp(dir, "/") == 0) {
	    //if the user didn't specify a target directory, then
	    chdir(getenv("HOME")); //change current dir to home dir
	    getcwd(path, 100);
	} 
	else if(strcmp(dir, "..") == 0)
	{
		chdir(dir); //change current dir to that dir.
		getcwd(path, 100);
	}
	else {
	    //if the user did specify a target directory, then
	    int err = chdir(dir); //change current dir to that dir.
	    if(err != -1) {
		    getcwd(path, 100);
		} else {
			printf("%s\n", strerror(errno));
		}
	}
}

void parseToArgv(char *argv[], char arr[], int *argc, int flag) 
{
	char word[100];
	int j = 0;
	int index = 0;

	for (int i = 0; 1; i++)
	{

		if(arr[i] == '"')
		{
			j = 0;
			int k = i+1;

			while(arr[k] != '"') {
				word[j++] = arr[k++];
				if(k == strlen(arr)) {
					printf("Invalid Command\n");
					return;
				}

			}

			i = k;
		}
		else if (arr[i] == ' ' || (flag == 1 && arr[i] == '&') || arr[i] == '\0')
		{
			word[j] = '\0';

			//empty string should not be accepted
			if(strcmp(word, "\0") == 0)	{

				if (i == strlen(arr)) 
					break;

				continue;
			}
			
			argv[index] = calloc(j+1, sizeof(char));
			strcpy(argv[index], word);
			index++;
			j = 0;
		
			if (i == strlen(arr)) 
				break;
		}
		else
			word[j++] = arr[i];
	}

	argv[index] = NULL;
	*argc = index;
}


FILE* inputFromFile(char *argv[]) 
{
  FILE * fp = NULL;
  int i = 1;

   //loop will execute until NULL found
  while(argv[i]) 
  {
    if(argv[i][0] == '<') 
    {
      	if(argv[i++]) 
      	{
			fp = fopen(argv[i], "r");

			open(argv[i], O_RDONLY);
			if(fp == NULL) {
		  		perror("Error while opening input redirect\n");
			}

			return fp;
      }
    }
    i++;
  }

  //if the user didn't want to redirect, we just return NULL.
  return fp;
}

FILE* outputToFile(char *argv[]) 
{
  	FILE *fp = NULL;
  	int i = 1;

  	while(argv[i]) 
  	{
		if(strcmp(argv[i], ">") == 0) 
		{
			int tempPosition = i;
		    if(argv[i++]) 
		    {
				fp = fopen(argv[i], "w");
				open(argv[i], O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
				if(fp == NULL) {
			  		perror("error while opening output redirect");
				}

				argv[tempPosition] = NULL;

				return fp;
			}
		}
		
		i++;
	}
	  
  	return fp;
}

int getPipePosition(char *argv[])
{
	int i = 0;
  	while(argv[i]) 
  	{
    	if(strcmp(argv[i], "|") == 0) 
    	{
      		argv[i] = NULL;
      		i++;
      
      		return i;
    	}

    	i++;
  	}

	return 0;
}

void pipeFuntion(char *argv[], int pos) 
{

	int pipeEnds[2];
	pipe(pipeEnds);

	//Second command
	char **argv2 = argv + pos; 
	
	if(fork() == 0) {
		
    	dup2(pipeEnds[0], STDIN_FILENO);

    	if(fork() == 0) 
    	{ 
	      	dup2(pipeEnds[1], STDOUT_FILENO);

	      	close(pipeEnds[0]);
	      	close(pipeEnds[1]);

			execvp(argv[0], argv);  
	    }

    	close(pipeEnds[1]);
    	close(pipeEnds[0]);

    	int status = 0;
    	wait(&status);
   
      	execvp(argv2[0], argv2);
  	}
	close(pipeEnds[1]);
	close(pipeEnds[0]);

	int status = 0;
	wait(&status);
}