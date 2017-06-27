#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

// Each function after main is described with a commented header
void childSig(int sig);																			
void status(int exitNum);																		
void resetInput(int * argc, char ** argv);
void exitShell();
void changeDir(int argc, char ** argv, char * path);
void makeProc(int argc, char ** argv, int *childExitMethod, struct sigaction action);
int isBack;
int childExitMethod = -5; 
int red;

int main() {
	dup2(1, 2);//added this
	int fileD, oldFD;  // used for file descriptor redirection
	char* curPath;		// holds HOME path
	curPath = getenv("HOME");
	int exitStatus = -5;	// chidl exit status variable
	char * argv[512];		// variable array of strings for storing parsed user input
	int argc = 0;			// holds count of parsed variables from input
	char input[2049] = " ";	// user input is initially stored in this string
	char * tok;				// used for tokenizing

	struct sigaction SIGINT_action, sC;			// These structs are used to make the shell ignore SIGINT and respond specifically to SIGCHILD
	SIGINT_action.sa_handler = SIG_IGN;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);	// sets SIGINT behavior


	sC.sa_handler = childSig;
	sigfillset(&sC.sa_mask);
	sC.sa_flags = 0;
	sigaction(SIGCHLD, &sC, NULL);				// sets SIGCHOLD behaviour

	/****************************************************************************************************************
	**	This section gets the user input and checks if it is a comment or blank space, if not, it converts the 
	**	input into the array of strings, argv, and increments argc for command processing.
	**
	****************************************************************************************************************/
	while (strcmp(input, "exit\n") != 0) {		// will loop until user inputs "exit"
		red = 0;
		resetInput(&argc, argv);				// function to ensure argv and argc are clear for new input
		isBack = 0;
		printf(":");							// each lines in shell starts with colon
		fgets(input, 2048, stdin);				// acquire user input
		if (strcmp(input, "exit\n") == 0) {		
			continue;
		}
		printf("\n");						
		if ((input[0] == '#') || strcmp(input, "\n") == 0) {	// makes shell do nothing for comments and empty lines
			continue;
		}
		tok = strtok(input, " ,\n");		// tok gets first string from input
		int i = 0;
		while (tok != NULL) {				// loops through user input
			if (tok[0] == '&') {			// sets isBack for later use to prevent the shell from waiting on child
				isBack = 1;
			}
			if (tok[0] == '>') {			// stdoutput redirection starts here
				fflush(stdout);
				tok = strtok(NULL, " ,\n");
				if ((fileD = open(tok, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
					printf("%s", "There was an error opening the file.");
					fflush(stdout);
					continue;
				}
				fflush(stdout);
				dup2(1, oldFD);				// stdout is redirected here
				if (-1 == dup2(fileD, 1))
				{
					perror("Can't dup2 stdout");
					fflush(stdout);
					continue;
				}
				red = 1;
				makeProc(argc, argv, &childExitMethod, SIGINT_action); // call to function that creates child
				dup2(oldFD, 1);	// return stdout
				continue;

			}
			if (tok[0] == '<') { // redirection code here
				tok = strtok(NULL, " ,\n");
				argv[i] = strdup(tok);
				argc++; 
				red = 1;
				makeProc(argc, argv, &childExitMethod, SIGINT_action);
				continue;

			}
			else {
				if (tok[0] != '&') {			// if & is encoutnered, it strips it out of the tokenizer so it isn't added to argv
					argv[i] = strdup(tok);	// argv is filled with parsed user input
					argc++;					// count is made for argv
					i++;
				}
				tok = strtok(NULL, " ,\n");	// gets next token from input string and jumps back up to beginning of while loop until NULL is encountered
			}
		}

		/****************************************************************************************************************
		**	This Section handles checking if any of the commands are the builtins. The check for proper argument count numbers before attempting
		**  to process the command.
		**
		****************************************************************************************************************/

		if (strcmp(argv[0], "status") == 0) {
			if (argc > 1) {					
				printf("%s\n", "status takes no arguments.");
				fflush(stdout);
				continue;
			}
			else {
				status(childExitMethod); // call to built-in status function
			}
		}
		else if (strcmp(argv[0], "exit") == 0) {
			if (argc > 1) {
				printf("%s\n", "exit takes no arguments.");
				fflush(stdout);
				continue;
			}
			else {
				exit(0); // call to built-in exit function
			}
		}
		else if (strcmp(argv[0], "cd") == 0) {
			changeDir(argc, argv, curPath); // call to built-in directory change function
		}

		else if (red == 0) {	// this processes the input if no redirection is specified

			argv[argc] = NULL;
			pid_t spawnpid = -5;
			spawnpid = fork();
			switch (spawnpid) {
			case -1:
				perror("Fork Error");
				fflush(stdout);
				exit(1);
				break;
			case 0:
				if (isBack == 0) {
					SIGINT_action.sa_handler = SIG_DFL;		// after the child is created, it is made to accept the default for SIGINT
					sigaction(SIGINT, &SIGINT_action, NULL);
				}
				execvp(*argv, argv); // call to execvp with argv as argument
				fflush(stderr);
				perror(argv[0]);	// if fails, print error
				fflush(stderr);
				exit(1);
				break;
			default:
				if (isBack == 1) {
					printf("background pid is %d\n", spawnpid);	// if child is background process, this is called to print pid
					fflush(stdout);
				}
				
			} 
			if (isBack == 0) {								// if foreground, waitpid is called on child
				waitpid(spawnpid, &childExitMethod, 0);
			}

		}
		else
		{
			continue;
		}

	}
	exit (0);
}
/**************************************************END OF MAIN**********************************************************/


/****************************************************************************************************************
**	Status Function: takes child exit number as argument and determined if the child was exited or signal interrupted.
**	
**
****************************************************************************************************************/
void status(int exitNum) {

	if (WIFEXITED(childExitMethod) != 0) {	// uses macro to determine if exit occured

		int exitStatus = WEXITSTATUS(childExitMethod);
		printf("exit value %d\n", exitStatus);
		fflush(stdout);
	}
	if (WIFSIGNALED(childExitMethod) != 0) {	// uses macro to determine if signal interrupt ended child
		int termSignal = WTERMSIG(childExitMethod);
		printf("terminated by signal %d\n", termSignal);
		fflush(stdout);
	}
	return;
}


/****************************************************************************************************************
**	Exit Function
**	
**
****************************************************************************************************************/
void exitShell() {
	exit(0);
}

/****************************************************************************************************************
**	Function to reset variables for shell from std input
**	
**
****************************************************************************************************************/
void resetInput(int * argc, char ** argv) {
	int i = 0;
	while (i < *argc) {
		free(argv[i]);  // frees previously allocated string pointers in argv
		i++;
	}
	*argc = 0; // reset argc
	return;
}


/****************************************************************************************************************
**	Function to process built-in "cd" functionalities. Checks number of arguments to ensure correct usage.
**	
**
****************************************************************************************************************/
void changeDir(int argc, char ** argv, char * path){
	if (argc > 2) {												// no more than two arguments are allowed
		printf("%s\n", "USAGE: cd <path to change to>.");
	
	}
	else if (argc == 2) {					// if two arguments are given, it checks to see if the path as given exists
		struct stat st;
		if (stat(argv[1], &st) == 0) {
			chdir(argv[1]);					// if it exists, chdir is called on it
		}
		else if (stat(argv[1], &st) != 0) {	// if not, it is assumed to be relative and is catenated to current directory
			fflush(stdout);
			strcat(path, "/");
			strcat(path, argv[1]);
			if (stat(path, &st) == 0) { // if the new combination exists, chdir is called on it.
				chdir(path);
			}
		}
	}
	else {
		chdir(getenv("HOME")); // if only cd is called, chdir is changed to HOME
	}
	return;
}

/****************************************************************************************************************
**	This function is identical to the code in main that forks. I tried swapping it, but it messed the functionality of my
**	code in a way where I couldn't figure out how to fix it without more time. 
**
****************************************************************************************************************/
void makeProc(int argc, char ** argv, int * childExitMethod, struct sigaction action) { 

	argv[argc] = NULL;
	pid_t spawnpid = -5;
	spawnpid = fork();
	switch (spawnpid) {
	case -1:
		perror("Fork Failed");
		fflush(stdout);
		exit(1);
		break;
	case 0:
		action.sa_handler = SIG_DFL;
		sigaction(SIGINT, &action, NULL);
		if (isBack == 1) {
			int backReDir;
			if ((backReDir = open("/dev/null", O_WRONLY | O_CREAT)) == -1) {
				printf("can't write to background file");
				perror("/dev/null open error");
				fflush(stdout);
				exit(1);
			}
			dup2(backReDir, 1);
		}

		execvp(*argv, argv);
		perror(argv[0]);
		fflush(stderr);
		exit(1);
		break;
	default:
		if (isBack == 1) {
			printf("background pid is %d\n", spawnpid);
			fflush(stdout);
		}
		break;
	}
	if (isBack == 0) {
		waitpid(spawnpid, &childExitMethod, 0);
	}
	return;


}

/****************************************************************************************************************
**	This function is passed to the SIGCHLD handler to check for interrupts.
**
**
****************************************************************************************************************/
void childSig(int signo) {
	wait(&childExitMethod);
	if ((WIFEXITED(childExitMethod) != 0) && (isBack == 1)) {	// uses macro to determine if exit occured

		int exitStatus = WEXITSTATUS(childExitMethod);
		printf("child exited with value %d\n", exitStatus);
		fflush(stdout);
	}
	if (WIFSIGNALED(childExitMethod) != 0) {
		int termSignal = WTERMSIG(childExitMethod);
		printf("terminated by signal %d\n", termSignal);
		fflush(stdout);
	}
	return;
}
