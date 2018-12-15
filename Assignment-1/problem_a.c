#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
extern int errno;

void parseToArgv(char *argv[], char arr[], int *argc);
void changeDirectory(char path[], char *dir);

int main() 
{
	char line[100];
	char *argv[100];

	for (int i = 0; i < 100; i++)
		argv[i] = NULL;

	int argc = 0;
	char path[100] = "";

	getcwd(path, 100);
	
	while (1) 
	{
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

		//paring line to arguments using space separation
		parseToArgv(argv, line, &argc);

		if (strcmp(argv[0], "cd") == 0) {
			changeDirectory(path, argv[1]);
		} 
		else 
		{

			if (fork() == 0) {
				execvp(argv[0], argv);
				printf("%s\n", strerror(errno));
			}
			else
			{
				int status=0;
				wait(&status);
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

void parseToArgv(char *argv[], char arr[], int *argc) 
{
	char word[100];
	int j = 0;
	int index = 0;

	for (int i = 0; 1; i++)
	{
		if (arr[i] == ' ' || arr[i] == '\0')
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
