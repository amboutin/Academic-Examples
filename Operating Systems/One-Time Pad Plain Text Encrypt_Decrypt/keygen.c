#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define true 1
#define false 0

int g_rand = true;
int g_val = 0;
int rangeRando(int, int);			// function that returns random numbers within a range

int main(int argc, char *argv[]) {

	int index = 0;				// used to insert each key letter into character array
	char genKey[70000] = "";	// buffer for generated key
	int size = 0;
	int temp;
	char alpha;
	int i = 0;

	if (argc != 2) {
		fprintf(stderr, "USAGE: %s keyLength\n", argv[0]); // checks for correct number of arguments
		exit(1); 
	}
	size = atoi(argv[1]);				// size turns the number passed in into an integer for use as size of key buffer
	for (i = 0; i < size; i++) {
		temp = rangeRando(64, 90);
		if (temp == 64) {				// 64 isn't the space character, but for this it is represented by it
			temp = 32;					// so if 64 is given, then temp will be set to 32, the actual numerical representation of space
			alpha = (char)temp;			// make a character out of the ASCII integer
			genKey[index] = alpha;		// put into key array
			index++;					// move to next spot in key array
			continue;
		}
		else {							// same as if statement above, but without the specific change for space. so its for the other letters
			alpha = (char)temp;			
			genKey[index] = alpha;
			index++;
			continue;
		}
	}
	alpha = '\n';		// the last character is a newline
	genKey[index] = alpha;
	printf("%s", genKey);
	exit(0);
}


/***********************************************
** Function that returns a random int within a specified range
**
************************************************/
int rangeRando(int floor, int ceiling) {

	if (g_rand == true) {
		srand(time(NULL));
		int rando = rand() % (ceiling + 1 - floor) + floor;
		g_rand = false;
		return rando;
	}
	else {
		srand(time(NULL));
		int rando = (rand() + g_val) % (ceiling + 1 - floor) + floor;
		g_val += 13;
		g_rand = true;
		return rando;
	}

}
