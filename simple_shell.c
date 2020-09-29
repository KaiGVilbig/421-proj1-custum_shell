#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "utils.h"

//Function to get the command from the user and store it in a file to be
//parsed and executed.
int readCmd(char **cmd)
{
	//create the dynamic array initially with size of char. this will
	//grow as the user types in more characters to the command.
	int c;
	int count;

	//get character from user input
	c = getchar();
	char* input = (char*)malloc(sizeof(char));
	int nextSize = sizeof(char);
	count = 0;

	//loops and gets character from user input until \n is entered.
	//each time a new character is entered, more memory is allocated to
	//the dynamic memory.
	while(c != 0x0A)
	{
		if(count > 0)
		{
			nextSize += sizeof(char);
			input = (char*)realloc(input, nextSize);
		}
		input[count] = c;
		count++;
		c = getchar();
	}

	//gets the number of arguments the user has entered. this is counts
	//used for the size of the arguments array later
	int argNum = count_spaces(input);

	cmd[0] = unescape(input, stderr);

	free(input);
	return argNum;
}

//Function to parse the command that has been saved to the file.
int parse(char **cmd, char **args, int cmdLen)
{
	int run = 1;
	//creates an array that will contain the command and the array that
	//contains the arguments
	int c;

	//need this to work for ./simple_shell?
	//if I dont put it the string gets corrupted but no leaks
	//if I put it, ./ works but there are leaks...
	char *tempArgs = (char*)malloc(sizeof(char));
	char *envi;// = (char*)malloc(sizeof(char));
	int count = 0;

	//parse the command. take which command to execute and put it in args[0]
	char *a = *cmd;
	tempArgs = strtok_r(a, " ", &a);
	tempArgs = (char*)realloc(tempArgs, (strlen(tempArgs) + 8));
	tempArgs += '\0';

	args[0] = tempArgs;

	//if command has parameters, throw them into args[1]
	if(cmdLen != 0)
	{
		args[1] = a;
	}

	//case where the user has entered to exit the program
	if(strcmp(args[0], "exit") == 0)
	{
		run = 0;
	}
	//setenv case
	else if(strcmp(args[0], "setenv") == 0)
	{
	        char *p = args[1];
	        envi = strtok_r(p, " ", &p);
		setenv(envi, p, 1);
	}
	//getenv case
	else if(strcmp(args[0], "getenv") == 0)
	{
		envi = getenv(args[1]);
		printf(envi);
		printf("\n");
	}
	//the user has either entered in a command that executes something, or an invalid
	//command has been entered
	else
	{
		//create a child process
		pid_t pid = fork();
		if(pid < 0)
		{
			fprintf(stderr, "fork failed");
		}
		//child process
		if(pid == 0)
		{
			//try to execute the command. if failed, error message will
			//be prompted
			if(execvp(args[0], args) < 0)
                        {
                                fprintf(stderr, "invalid command entered: ");
				printf(args[0]);
				printf("\n");
                        }
			exit(0);
		}
		//parent process
		//wait for the child process to finish executing the command
		if (pid > 0)
		{
			wait(0);
		}
	}
	tempArgs = NULL;
	args[0] = NULL;
	return run;
}

//int execute(char **args)
//{
//	int run = 1;
//        if(strcmp(args[0], "exit") == 0)
//        {
//                run = 0;
//        }
//        else
//        {
//                pid_t pid = fork();
//                if(pid < 0)
//                {
//                        fprintf(stderr, "fork failed");
//                }
//                if(pid == 0)
//                {
//                        if(execvp(args[0], args) < 0)
//                        {
//                                fprintf(stderr, "invalid command entered: ");
//                                printf(args[0]);
//                        }
//                        exit(0);
//                }
//                if (pid > 0)
//                {
//                        wait(0);
//                }
//        }
//      free(tempArgs);
//        return run;

//}

//Loop function that continues to loop until the user quits
void loop()
{
	int argsq;
	int running = 1;
	char *cmd[1];
	do
	{
		//get the command from the user and get how many arguments
		//are in the command so that the args array can be made
		printf("> ");
		argsq = readCmd(cmd);
		//create the arguments array based on if the command has
		//more that one arguments
//		if(argsq != 0)
//		{
//			argsq = 1;
//		}
//		char *args[(argsq + 1)];
		char *args[] = {NULL, NULL, NULL};
		//parse the command. This also executes the command as I was having issues w the execute function
		running = parse(cmd, args, argsq);

		free(cmd[0]);
		//execute the command
		//printf(args[0]);
		//running = execute(args);
		printf("\n");

	}while(running != 0);
}

void main()
{
	//start the loop that runs until the user quits
	loop();
}
