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

#define BUFFSIZE 70000

void error(const char *msg) { perror(msg); } // Error function used for reporting issues

void childSig(int signo);					 // SIGSHLD handler

int childExitMethod = -5;

int getAlphNum(char *array, char value);    // function converts letters to numbers for modulo math

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten;
	socklen_t sizeOfClientInfo;
	char buffer[BUFFSIZE];
	char buffer2[BUFFSIZE];
	char plain[BUFFSIZE];
	char alphabet[] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' ' };
	struct sockaddr_in serverAddress, clientAddress;

	char alpha, verify[] = "C";

	struct sigaction sC;			// This is to make the parent wait on its children when the children are ready
	sC.sa_handler = childSig;
	sigfillset(&sC.sa_mask);
	sC.sa_flags = 0;
	sigaction(SIGCHLD, &sC, NULL);				// sets SIGCHLD behaviour

	if (argc < 2) { fprintf(stderr, "USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args
    
	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process
												// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) {
		error("otp_dec_d: ERROR opening socket");
		exit(1);
	}

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
	{
		error("otp_dec_d: ERROR on binding");
		exit(1);
	}
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

    /*************************************************************************************************/

	// Create infinite loop to prevent server from exiting after work
	for (;;) {

		// Accept a connection, then Fork connection to child
		do {
			sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
			establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		} while (establishedConnectionFD < 0);

		pid_t spawnpid = -5;
		spawnpid = fork();
		switch (spawnpid) {
		case -1:
			perror("otp_dec_d: Fork Error");
			fflush(stdout);
			exit(1);
			break;
		case 0:
			close(listenSocketFD); // Close the listening socket


	 /*************************************************************************************************/
			 // Validate that it has connected to correct server
			do {
				charsRead = recv(establishedConnectionFD, verify, sizeof(verify), 0); // Read the client's message from the socket
			} while (charsRead < 0);
			if (verify[0] == 'c') {
				do {
					verify[0] = 'C';
					charsWritten = send(establishedConnectionFD, verify, strlen(verify), 0); // Write to the server
				} while (charsWritten < 0);
			}
			else {
				do {
					verify[0] = 'C';
					charsWritten = send(establishedConnectionFD, verify, strlen(verify), 0); // Write to the server
					error("otp_dec_d: Incompatible Connection Attempt with otp_enc");
					exit(1);
				} while (charsWritten < 0);
			}

			/*************************************************************************************************/

			// Get Plaintext from Client
			memset(buffer, '\0', 256);
			while (alpha != '*') {

				charsRead = recv(establishedConnectionFD, buffer, BUFFSIZE - 1, 0); // Read the client's message from the socket
				alpha = buffer[strlen(buffer) - 1];
			}

			/*************************************************************************************************/

			// Get key from Client
			alpha = ' ';
			sleep(1);
			memset(buffer2, '\0', BUFFSIZE);
			while (alpha != '*') {

				charsRead = recv(establishedConnectionFD, buffer2, BUFFSIZE - 1, 0); // Read the client's message from the socket
				alpha = buffer2[strlen(buffer2) - 1];
			}

			/*************************************************************************************************/

			// Decrypt cyphertext 
			int i, result;
			memset(plain, '\0', 256);
			for (i = 0; i < (strlen(buffer) - 1);i++) {

			result = (getAlphNum(alphabet, buffer[i]) - getAlphNum(alphabet, buffer2[i]));
			if (result < 0) {
			result += 27;
			}
			result = result % 27;
			plain[i] = alphabet[result];
			}
			plain[strlen(plain)] = '*';

			/*************************************************************************************************/

			// Send Plaintext to Client
			while (charsWritten != strlen(plain)) {
				charsWritten = send(establishedConnectionFD, plain, strlen(plain), 0); // Write to the Client
			}
			exit(1);

		default:
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			break;
		}
	}
}

int getAlphNum(char *array, char value) {
	int i;
	for (i = 0; i < 27; i++) {
		if (array[i] == value) {
			return i;
		}
	}
	return -1;
}

/****************************************************************************************************************
**	This function is passed to the SIGCHLD handler to wait on children when SIGCHLD is sent.
**
****************************************************************************************************************/
void childSig(int signo) {
	wait(&childExitMethod);
}