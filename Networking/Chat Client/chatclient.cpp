/******************************************************************************************************
**	Name: Aaron Boutin		Assignment: Project 1			Date: 11/FEB/2017
**	
**	References/Sources: ChatClient was influenced by code from CS 344, specificaly socket and threading instruction.
**						I refered to an encryption program that I wrote for CS 344 for sending and recieving. However, 
**					 	I did include the use of string objects in addition to previously practised use of c style strings.
**
**	Extra Credit: I have the progam use threading to implement concurrent send/recieve between client and host
******************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <pthread.h>

#define BUFFSIZE 501

void error(const char *msg) { perror(msg); } 																	// Error function used for reporting issues
void sendLoop(int socketFD, char* handle);
void *recvLoop(void *socketFD);
int startUp(char*[]);

struct multArgs{																								// struct passed to thread call for function recvLoop()
	int socket;
	char servHandle[BUFFSIZE];
};

int main(int argc, char *argv[])
{
	int socketFD, charsWritten, charsRead, resultInt;
	char buffer[BUFFSIZE];
	char handleBuffer[BUFFSIZE];
	pthread_t myThreadID;
    
	if (argc != 3 ) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } 									// Check usage & args

	socketFD = startUp(argv);																					// Function creates socket and connects to server, then verifies.

	/*************************************************************************************************/
	// Get Handle for both Client and Server
	// I decided that sending and recieving of handles was inefficient for every send/recieve, so I have each end send it once and then they themselve append it to text.
	std::cout << "Welcome to A-CHAT" << std::endl << "Please Enter a name for the Handle you would like to use: ";
	std::cin >> handleBuffer;
	do {
		charsWritten = send(socketFD, handleBuffer, strlen(handleBuffer), 0);									 // Write to the server
	} while (charsWritten < 0);

	std::cout << "Please wait while the Server's Handle is acquired." << std::endl;
	memset(buffer, '\0', BUFFSIZE);
	do {
		charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 												// Read the server's message from the socket
	} while (charsRead < 0);
	fflush(stdout);
	fflush(stdin);
	std::cout << "You may begin chatting" << std::endl;

	/*************************************************************************************************/
	// Setup and initiate thread that handles recieving from server end
	struct multArgs args;
	args.socket = socketFD;
	strcpy(args.servHandle, buffer);
	resultInt = pthread_create(&myThreadID, NULL, recvLoop, &args);												// Run the Revieve Loop function in separate thread for concurrent communication

	sendLoop(socketFD, handleBuffer);																			// Put user in active loop for sending text
	pthread_cancel(myThreadID);																					// If client ends conversation before server, recieve thread is cancelled.

	/*************************************************************************************************/
	close(socketFD); // Close the socket
	exit(0);
}

/******************************************************************************************************
**	SENDLOOP
**	
**	Function takes a socketId and client handle string as input. 
**	This function only handles the transmittal of client data to the server. 					
**	Was designed for use with a recieving loop in a thread to run concurrently.
******************************************************************************************************/
void sendLoop(int socketFD, char* handle){
	char input[501];
	int charsWritten2;
	while (strcmp(input, "\\quit") != 0){
		std::cout << handle << "> ";
		std::cin.getline (input,500);
		do {
			charsWritten2 = send(socketFD, input, strlen(input), 0);										 	// Write to the server
		} while (charsWritten2 < 0);
	}
}

/******************************************************************************************************
**	RECVLOOP
**	
**	Void function was designed specifically for use in being called in a thread. This allows concurrent use with
**	a sending function. This function takes as input a struct with two arguments, an integer the represents
**	the socket file descriptor for output, and a character array with the server handle for prepending. 
******************************************************************************************************/
void* recvLoop(void* argument){
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	struct multArgs* arg = (struct multArgs*) argument;
	char output[501] = {0};
	std::string modOut;
	int charsRead;
	while (strcmp(output, "\\quit") != 0){
		memset(output, '\0', 501);
		do {
			charsRead = recv(arg->socket, output, sizeof(output), 0); 											// Read the server's message from the socket
		} while (charsRead < 0);
		std::cout << std::endl;
		modOut.assign(arg->servHandle);																			// append handle to text
		modOut.append("> ");
		modOut.append(output);

		std::cout << modOut << std::endl;
		int test = strcmp(output, "\\quit");
		if (strcmp(output, "\\quit") == 0)
		{
			do {
			charsRead = send(arg->socket, output, strlen(output), 0); 											// Write to the server to end connection
			}while (charsRead < 0);
			std::cout << "Server has ended conversation, please enter \\quit to close program." << std::endl;
		}
		fflush(stdout);
	}
}

/******************************************************************************************************
**	STARTUP
**	
**	This function handles the creation of a client socket as well as server-client verification.
**	Takes the arguments from the call of the program as hostname and port number, being arguments 1 and 2, 
**	respectively. Returns the socket file descriptor for use in input/output.
******************************************************************************************************/
int startUp(char *argv[]){
	char verify[] = "PORTNUM";	
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	int socketFD, portNumber, charsWritten, charsRead;
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));	 												// Clear out the address struct
	portNumber = atoi(argv[2]); 																				// Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; 																		// Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);																	// Store the port number
	serverHostInfo = gethostbyname(argv[1]);																	// Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)serverHostInfo->h_addr, (char*)&serverAddress.sin_addr.s_addr, serverHostInfo->h_length); 


	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)							// Connect socket to address
	{
		fprintf(stderr, "CLIENT: ERROR connecting to port %d\n", portNumber); 
		exit(2);
	}
	do {
		charsWritten = send(socketFD, verify, strlen(verify), 0);												// Write to the server
	} while (charsWritten < 0);
	memset(verify, '\0', 20);
	do {
		charsRead = recv(socketFD, verify, sizeof(verify), 0);													 // Read the server's message from the socket
	} while (charsRead < 0);
	if (strcmp(verify, "PORTNUM") != 0) { error("Chatclient: Incompatible Connection Attempt");exit(1); }
	return socketFD;
}