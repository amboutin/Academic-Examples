/******************************************************************************************************
**	Name: Aaron Boutin		Assignment: Project 2			Date: 10/MAR/2017
**	
**	References/Sources: ftserver was influenced by code from CS 344, specifically socket and threading instruction.
**						I refered to an encryption program that I wrote for CS 344 for sending and recieving. I also  
**					 	relied on the website stackoverflow for many semantic/syntax issues. 
**
**	Extra Credit: Server is Multi-Threaded
******************************************************************************************************/

#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <netdb.h> 
#include <dirent.h>

#define BUFFSIZE 70000

void error(const char *msg) { perror(msg); } // Error function used for reporting issues

void childSig(int signo);					 // SIGSHLD handler

int childExitMethod = -5;

int startUp(char* argv);

void handleRequest(int establishedConnectionFD, char *clientCommand, char *clientDataPort, char *clientHostName, char *clientFileName );

int main(int argc, char *argv[])
{
	struct hostent* serverHostInfo;
	struct sockaddr_in clientAddress;
	int listenSocketFD, establishedConnectionFD, charsRead, charsWritten, argCount;
	socklen_t sizeOfClientInfo;
	char buffer[BUFFSIZE];
	char alpha, verify[] = "C";
	char * tok,	*clientHostName, *clientCommand, *clientDataPort, *clientFileName;	// Used for parsing client requests

	struct sigaction sC;			// This is to make the parent wait on its children when the children are ready
	sC.sa_handler = childSig;
	sigfillset(&sC.sa_mask);
	sC.sa_flags = 0;
	sigaction(SIGCHLD, &sC, NULL);	// sets SIGCHLD behaviour


    /*************************************************************************************************
    ** This section validates user unput, so that there is one argument and that is a number in the port range 1025 to 65535 inclusive
	*************************************************************************************************/    
	if (argc < 2) { fprintf(stderr, "USAGE: %s port\n", argv[0]); exit(1); } 										// Check usage & args
    
	if (atoi(argv[1]) <= 1024 || atoi(argv[1]) >= 65536){
		fprintf(stderr, "This program only accepts ports numbers from 1025 to 65535\n");
		exit(1);
	}

	/*************************************************************************************************
	** next we call the startUp function, which returns the socket file descriptor for use
	*************************************************************************************************/	
	listenSocketFD = startUp(argv[1]);

	/*************************************************************************************************
	** This next section allows for multiple connections to the server, in which each connection
	** is forked into a child process that handles the request.
	*************************************************************************************************/	
	for (;;) {																										// Create infinite loop to prevent server from exiting after work
																													
		do {
			sizeOfClientInfo = sizeof(clientAddress); 																// Get the size of the address for the client that will connect
			establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept a connection, then Fork connection to child
		} while (establishedConnectionFD < 0);

		pid_t spawnpid = -5;
		spawnpid = fork();
		switch (spawnpid) {																							// Switch statement that deals with forks
		case -1:
			perror("ftserver: Fork Error\n");
			fflush(stdout);
			exit(1);
			break;
		case 0:
			close(listenSocketFD); 																					// Close the listening socket for the child

	/****************************************************************************************************************
	**	This section verifies that the client connecting to it is the desired client program, 
	**	if not, then an incorrect value is sent back.
	****************************************************************************************************************/
			do {
				charsRead = recv(establishedConnectionFD, verify, sizeof(verify), 0); 			// Read the client's message from the socket
			} while (charsRead < 0);
			if (verify[0] == 'c') {
				do {
					verify[0] = 'C';
					charsWritten = send(establishedConnectionFD, verify, strlen(verify), 0); 	// Write to the server
				} while (charsWritten < 0);
			}
			else {
				do {
					verify[0] = 'D';
					charsWritten = send(establishedConnectionFD, verify, strlen(verify), 0);	 // Write to the server
					error("ftserver: Incompatible Connection Attempt with ftclient\n");
					exit(1);
				} while (charsWritten < 0);
			}

	/****************************************************************************************************************
	**	This section takes the client's request and parses it into appropriate variables for request processing 
	**	
	****************************************************************************************************************/
			memset(buffer, '\0', 256);
			do {
				charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0); // Read the client's message from the socket
			} while (charsRead < 0);
			tok = strtok(buffer, " ,\n");
			argCount = atoi(tok);
			tok = strtok(NULL, " ,\n");	
			clientHostName = strdup(tok);
			tok = strtok(NULL, " ,\n");
			clientCommand = strdup(tok);	
			if(argCount == 6){
				tok = strtok(NULL, " ,\n");
				clientFileName = strdup(tok);
			}
			tok = strtok(NULL, " ,\n");
			clientDataPort = strdup(tok);
			printf("Established connection from %s.\n",clientHostName);

	/****************************************************************************************************************
	**	Now we call our handleRequest function, which routes the computer appropriately to handle the client request
	**	
	****************************************************************************************************************/
			handleRequest(establishedConnectionFD, clientCommand, clientDataPort, clientHostName, clientFileName );

		default:
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			break;

		}//switch statement
	}//serve loop
}//  End of Main, no exit function since this is designed to serve until interrupted


/****************************************************************************************************************
**	This function is passed to the SIGCHLD handler to wait on children when SIGCHLD is sent.
**
****************************************************************************************************************/
void childSig(int signo) {
	wait(&childExitMethod);
}


/****************************************************************************************************************
**	startUp()
**	
**	This function handles creating the inital listening socket, allowing up to 5 simultaneous connection.
**
**	RETURNS the socket that the server listens on for acceptance by client connections.
**
****************************************************************************************************************/	
int startUp(char* argv){
	int listenSocketFD, portNumber;		
	struct sockaddr_in serverAddress;
	memset((char *)&serverAddress, '\0', sizeof(serverAddress));							 // Clear out the address struct
	portNumber = atoi(argv); 																 // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET;														 // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); 											 // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; 											 // Any address is allowed for connection to this process
	// Create the socket						
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); 										 
	if (listenSocketFD < 0) {
		error("ftserver: ERROR opening socket\n");
		exit(1);
	}
	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
	{
		error("ftserver: ERROR on binding\n");
		exit(1);
	}
	printf("Server open on %s\n", argv);
	listen(listenSocketFD, 5); 																// Flip the socket on - it can now receive up to 5 connections
	return listenSocketFD;
}

/****************************************************************************************************************
**	handleRequest()
**	
**	This function deals with either transmiting the directory contents, or transmitting the text file
**
**	TAKES the connection file descriptor to send to, as well as pointers to strings for various client information from its request:
**	Command, DataPort, HostName, and the name of the requested file.
**
****************************************************************************************************************/	
void handleRequest(int establishedConnectionFD, char *clientCommand, char *clientDataPort, char *clientHostName, char *clientFileName ){
	
			char filenames[BUFFSIZE];
			struct hostent* serverHostInfo;
			int charsWritten, portNumber;
			FILE * fileD;
			char buffer2[BUFFSIZE];
			struct sockaddr_in serverAddress;
			char command[] = "l";
			if(strcmp(clientCommand, "-l") == 0){
				printf("List Directory requested on port %s.\n", clientDataPort);
				printf("Sending directory contents to %s:%s\n", clientHostName, clientDataPort);

			   memset(filenames, '\0', BUFFSIZE);
		       struct dirent **namelist;
		       int n;
		       n = scandir(".", &namelist, NULL, alphasort);
		       if (n < 0){
		           perror("scandir\n");
		       }
		       int i;
		       while (n--) {	       	
		               strcat(filenames,namelist[n]->d_name);
		               strcat(filenames, " ");
		               free(namelist[n]);
		           }
		       free(namelist);
			}
			else{
				printf("File \"%s\" requested on port %s.\n", clientFileName, clientDataPort);
				command[0] = 'g';
				if ((fileD = fopen(clientFileName, "r")) == NULL) {
					printf("File not found. Sending error message to %s:%s\n", clientHostName, clientDataPort);
					fflush(stdout);
					command[0] = 'e';
					do {
						charsWritten = send(establishedConnectionFD, command, strlen(command), 0); 	// Write error to the client
					} while (charsWritten < 0);
					exit(2);
			}
			do {
				charsWritten = send(establishedConnectionFD, command, strlen(command), 0); 			// Write error to the client
			} while (charsWritten < 0);
			printf("Sending \"%s\" to %s:%s\n", clientFileName, clientHostName, clientDataPort);
		}


/*************************************************************************************************/
			// code to connect to client data port
		sleep(2);														  // give client side time to setup server socket for data connection
		// Set up the server address struct
		memset((char *)&serverAddress, '\0', sizeof(serverAddress)); 	  // Clear out the address struct
		serverAddress.sin_family = AF_INET;								  // Create a network-capable socket
		serverAddress.sin_port = htons(portNumber); 					  // Store the port number
		serverAddress.sin_addr.s_addr = INADDR_ANY;						  // Any address is allowed for connection to this process
		portNumber = atoi(clientDataPort);								  // Get the port number, convert to an integer from a string
		serverAddress.sin_port = htons(portNumber); 					  // Store the port number
		serverHostInfo = gethostbyname("localhost");					  // Convert the machine name into a special form of address
		memcpy((char*)serverHostInfo->h_addr, (char*)&serverAddress.sin_addr.s_addr, serverHostInfo->h_length); // Copy in the address

		// Set up the socket
		int socketFD = socket(AF_INET, SOCK_STREAM, 0); 				  // Create the socket
		// Connect to server
		if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		{
			fprintf(stderr, "ftserver: ERROR connecting to port %d\n", portNumber);
			exit(2);
		}

	/*************************************************************************************************/
		if (command[0] == 'l'){
			do {
				charsWritten = send(socketFD, filenames, strlen(filenames), 0); 
			} while (charsWritten < 0);
		}
		if (command[0] == 'g'){
			memset(buffer2, '\0', BUFFSIZE);
			while(fgets(buffer2, BUFFSIZE, fileD) != NULL){
				fflush(stdout);
				//usleep(500);
				while (charsWritten != strlen(buffer2)) {
					charsWritten = send(socketFD, buffer2, strlen(buffer2), 0); 
				}
				charsWritten = 0;
				memset(buffer2, '\0', BUFFSIZE);
			}
			fclose(fileD);		
		}
		fflush(stdout);
		sleep(1);		
		close(socketFD);
		printf("Data Transport Complete\nAwaiting New Connection\n");
		fflush(stdout);			


}