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

#define BUFFSIZE 70000

void error(const char *msg) { perror(msg); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[BUFFSIZE];
	char buffer2[BUFFSIZE];
	char alpha, verify[] = "d";

	FILE * fileName;
    
	if (argc < 4) { fprintf(stderr,"USAGE: %s textfile keyfile port\n", argv[0]); exit(1); } // Check usage & args

	/*************************************************************************************************/

	// Get Plaintext from file
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	fileName = fopen(argv[1], "r");
	fgets(buffer, sizeof(buffer) - 1, fileName);
	fclose(fileName);
	buffer[strcspn(buffer, "\n")] = '*'; // Remove the trailing \n that fgets adds

	// Validate plaintext against bad characters
	int i;
	for (i = 0; i < strlen(buffer) - 1; i++) {
		if (buffer[i] < 'A' || buffer[i] > 'Z') {
			if (buffer[i] != ' ')
			{
				fprintf(stderr, "otp_enc error : %s contains bad characters", argv[1]);

				exit(1);
			}
		}
	}

	/*************************************************************************************************/

	// Get key from file
	memset(buffer2, '\0', sizeof(buffer2)); // Clear out the buffer array
	fileName = fopen(argv[2], "r");
	fgets(buffer2, sizeof(buffer2) - 1, fileName);
	fclose(fileName);
	buffer2[strcspn(buffer2, "\n")] = '*'; // Remove the trailing \n that fgets adds and replace with identifying character used to indicate end of string for transmission

	// Check that key is long enough
	if (strlen(buffer) > strlen(buffer2)) {
		fprintf(stderr,"Error: key '%s' is too short", argv[2]);
		exit(1);
	}

	/*************************************************************************************************/

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);		// Store the port number
	serverHostInfo = gethostbyname("localhost");	// Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)serverHostInfo->h_addr, (char*)&serverAddress.sin_addr.s_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
	{
		fprintf(stderr, "CLIENT: ERROR connecting to port %d\n", portNumber); 
		exit(2);
	}

	/*************************************************************************************************/
	
	// Validate that it has connected to correct server
	do {
		charsWritten = send(socketFD, verify, strlen(verify), 0); // Write to the server
	} while (charsWritten < 0);
	do {
		charsRead = recv(socketFD, verify, sizeof(verify), 0); // Read the client's message from the socket
	} while (charsRead < 0);
	if (verify[0] != 'D') { error("otp_enc: Incompatible Connection Attempt with otp_dec_d");exit(1); }

	/*************************************************************************************************/

	// Send Plaintext to otp_enc_d
	while (charsWritten != strlen(buffer)) {
		charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	}

	/*************************************************************************************************/

	// Send key to otp_enc_d
	charsWritten = 0;
	while (charsWritten != strlen(buffer2)) {
		charsWritten = send(socketFD, buffer2, strlen(buffer2), 0); // Write to the server
	}

	/*************************************************************************************************/

	// Get CypherText from otp_enc_d
	alpha = ' ';
	memset(buffer2, '\0', BUFFSIZE);
	while (alpha != '*') {

		charsRead = recv(socketFD, buffer2, sizeof(buffer2) - 1, 0); // Read the client's message from the socket
		alpha = buffer2[strlen(buffer2) - 1];
	}
	buffer2[strcspn(buffer2, "*")] = '\0'; // Remove the trailing identifier and replaces with NULL terminator
	fprintf(stdout,"%s\n", buffer2);

	/*************************************************************************************************/

	close(socketFD); // Close the socket
	exit(0);
}