#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//these are ascii codes to change text colour
//for error readability
#define RED "\033[0;31m"
#define RESET "\e[0m"
#define GREEN "\033[0;32m"

#define BUFFER_SIZE 1024

//WelcomeMessage -displays on open
void welcomePrompt()
{
	char* welcome = "Harvey Thompson Shell";
	printf("#####################\n");
	printf("%5s\n", welcome);
	printf("#####################\n");
}

//Prompt to be displayed
//Prints working directory
void prompt()
{
	char dir[100];
	printf(GREEN "%s" RESET "<<", getcwd(dir, 100));
}

//Function to get line from user
char* getLine()
{
	//string position	
	int pos=0;
	//initalise memory for buffer
	char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
	int c;
	
	//loop continutes untill string is over
	//if EOF or newline then replace with NULL terminator
	//then  return string
	//else insert character into string at postion
	//incriment positon and run loop again
	while(1)
	{
		c=getchar();
		if (c==EOF || c=='\n')
		{
			buffer[pos] = '\0';
			return buffer;
		}
		else
		{
			buffer[pos]=c;
		}
		pos++;
	}		
}


//Function to split command into seperate string
//stores them in a 2d pointer array
char** splitLine(char * line)
{
	//initalise variables
	int pos=0;
	char** tokens = malloc(BUFFER_SIZE * sizeof(char*));
	char* token;
	
	//strtok takes two arugements
	//the command we want to split and the delimiter we are using
	token= strtok(line, " ");
	//runs untill command is split and no more tokens can be got
	while (token != NULL)
	{
		tokens[pos] = token;
		//NULL means continue where left off on string
		token = strtok(NULL, " ");
		//printf("%s,", tokens[pos]);
		pos++;
	}
	//set last postion to NULL for exec function used later
	tokens[pos] = NULL;
	
	return tokens;
	
}

//function to add path
//takes arguement from 0 element of token array
//which contains the command for shell to run
char* addPath(char* arg)
{
	//variable for path
	char* path= "/bin/";
	//allocate memory for command
	char* command = malloc(sizeof(char) * BUFFER_SIZE);
	
	//strcpy copies contents of path into
	strcpy(command, path);
	//strcat adds contents of arg into command
	strcat(command, arg);
	
	//printf("PATH=%s\n", command);
	//printf("LEN=%ld\n", strlen(command));
	
	return command;
}

//function to execute command
//take command path and args array to be executed
int execute(char* command, char** args)
{
	if (strcmp(args[0], "exit") ==0)
	{
	return 0;
	}
	if (strcmp(args[0], "cd") ==0)
	{
		if (chdir(args[1]))
			{
			perror(RED "Directory not found\nTry full path" RESET);
			return EXIT_FAILURE;
			}
		else
		{
			return 1;
		}
	}
	
	
	pid_t pid;
	int status;
	
	//create new process with fork
	pid=fork();
	
	//new process has a pid of 0
	//if pid less than 0 an error has occured
	if (pid<0)
	{
		perror(RED "forking error" RESET);
		return EXIT_FAILURE;
	}
	//fork successful 
	else if (pid==0)
	{
		//execvp takes our command which is a bin file
		//and our 2d pointer array of any tokens which is null terminated
		//execv returns negative if execution failed- execvp error
		if (execvp(command, args)<0)
		{
		perror(RED "execvp erro" RESET);
		return EXIT_FAILURE;
		}	
	}
	else
	{
		//waitpid is syscall that waits for change in status of current pid
		if (waitpid(pid, &status, 0) <0)
		{
		perror(RED "waitpid error" RESET);
		return EXIT_FAILURE;
		}
	}
	return 1;
	
}



//loop functions allows multiple commands to be entered
//controlled by status variable which is returned by execute()
//0 if exit is typed else 1
void loop ()
{
	int status=1;
	do
	{
		prompt();
		char* myLine= getLine();
		//printf("MYLINE =%s\n", myLine);
		char** args= splitLine(myLine);
		char* command= addPath(args[0]);
		status = execute(command, args);
		//free memory
		free(myLine);
		free(args);
		free(command);
	}
	while(status);
	}

int main()
{
	welcomePrompt();
	loop();
	return EXIT_SUCCESS;
}


