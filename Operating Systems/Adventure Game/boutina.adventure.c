// Created by Aaron Boutin

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define true 1
#define false 0


struct Room{				// struct to hold room data
char * nameArr[15];			// array of char arrays holds room name in [0], the rest are connections - randomly allocated
int curConns;				// holds number of room connections - randomly allocated
char typeArr[15];			// char array to hold room type
};

int g_rand = true;			// global used for assiting random number generation for quicker processing
int g_val = 0;				// global used for assiting random number generation for quicker processing

void generateRooms(struct Room *);	// function that dynamically and randomly generates rooms

int rangeRando(int, int);			// function that returns random numbers within a range

char *fNames[] = {					// array of char arrays that holds names for file generation
	"ROOM1",
	"ROOM2",
	"ROOM3",
	"ROOM4",
	"ROOM5",
	"ROOM6",
	"ROOM7"
};

char *allNames[] = {				// ten room names
	"Dagobah",
	"Tatooine",
	"Hoth",
	"Coruscant",
	"Bespin",
	"Corellia",
	"Kashyyyk",
	"Naboo",
	"Alderaan",
	"Endor"
};

char *rNames[7];					// array of char arrays to hold 7 random room names

char *rTypes[] = {					// array of char arrays to hold room types
	"START_ROOM",
	"MID_ROOM",
	"END_ROOM"
};

int main(void){

	int i, j;
	int rando;				// holds randomly generated values
	char randoPick[15];		// holds randomly selected names
	int isThere;			// boolean for loop functionality
	int nameCount = 0;		// holds number of names selected and assigned to rNames[]
	for (i = 0; i < 7; i++) {
		rNames[i] = malloc(15 * sizeof(char));		// dynamically allocated 7 char arrays to hold selected names
	}
	for (i = 0; i < 7; i++) {
		isThere = true;
		while (isThere == true) {
			isThere = false;
			rando = rangeRando(0, 9);
			strcpy(randoPick, allNames[rando]);				// randomly gets name from total list
			for (j = 0; j < 7; j++) {						// loop checks if name has already been added
				if (strcmp(randoPick, rNames[j]) == 0) {
					isThere = true;
				}
			}
			if (isThere == false) {							// if name not in array, adds and increments nameCount, which is a placekeeper in the array
				strcpy(rNames[nameCount], randoPick);
				nameCount++;
			}
		}
	}

int dirPid = getpid();		
char dirName[30];
sprintf(dirName, "./boutina.rooms.%d/", dirPid);		// adds PID to pathname for directory creation
mkdir(dirName, 0777);									// creates directory

struct Room rooms[7];									// declare array of Struct Room

generateRooms(rooms);									// generate rooms for referenced struct array
/**********************************************************************************************
**	This section creates 7 files, iterating through the list of applicable file names.
**	It then prints the data for each room to each file.
**********************************************************************************************/
FILE *file;
for (i = 0; i < 7; i++) {

	char fileNames[30];
	strcpy(fileNames, dirName);
	strcat(fileNames, fNames[i]);
	file = fopen(fileNames, "a");
	fprintf(file, "ROOM NAME: %s\n", rooms[i].nameArr[0]);
	int index = 1;
	for (j = 1; j <= rooms[i].curConns; j++) {
		
		fprintf(file, "CONNECTION %d: %s\n", index, rooms[i].nameArr[j]);
		index = index + 1;
	}
	fprintf(file, "ROOM TYPE: %s\n", rooms[i].typeArr);
	fclose(file);
}
/***********************************************************************************************
**	This section takes the files printed and reads them into a NEW array of struct Room, one file per room.
**	It reads each line, tokenizing and strcmp for key words to identify what to do with them.
***********************************************************************************************/
struct Room readRooms[7];
for (i = 0; i < 7; i++) {
	readRooms[i].curConns = 0;
	for (j = 0; j < 9; j++) {  
		readRooms[i].nameArr[j] = malloc(15 * sizeof(char));
		strcpy(readRooms[i].nameArr[j], " ");
	}
}

for (i = 0; i < 7; i++) {

	char fileNames[30];
	strcpy(fileNames, dirName);
	strcat(fileNames, fNames[i]);
	file = fopen(fileNames, "r");
	char rSTok[30];
	char * tok;
	int ch;
	while (fgets(rSTok, 30, file)) {
		tok = strtok(rSTok, " :");
		if (strcmp(tok, "ROOM") == 0) {
			tok = strtok(NULL, " :");
			if (strcmp(tok, "NAME") == 0) {
				tok = strtok(NULL, " : \n \r");
				strcpy(readRooms[i].nameArr[0], tok);
			}
			if (strcmp(tok, "TYPE") == 0) {
				tok = strtok(NULL, " : \n \r");
				strcpy(readRooms[i].typeArr, tok);
			}
		}
		if (strcmp(tok, "CONNECTION") == 0) {
			tok = strtok(NULL, " :");
			ch = atoi(tok);
			tok = strtok(NULL, " : \n \r");
			strcpy(readRooms[i].nameArr[ch], tok);
			readRooms[i].curConns = readRooms[i].curConns + 1;
		}
	}
	fclose(file);
}
/***************************************************************************************************************
**	This is the gameplay code. It seaches the room struct for the START_ROOM, then sets variables to hold its data.
**	It then uses loops to search and compare possible connections with entered input. It then updates variables with 
**	the next room information, prints it all again, and continues.
***************************************************************************************************************/
char curRoom[15];
char *posRooms[9];
int posTemp;		// holds current room location in rooms array
int posCount;		// holds current room connection count
int isValid;
int stepCount = 0;	// tallies steps taken
char curType[15];	// holds current room type
char * pathTaken[20];	// collects path taken
int pTakenCount = 0;	// index for path taken array

for (i = 0; i < 7; i++) {										// searches for start room
	if (strcmp(readRooms[i].typeArr, "START_ROOM") == 0) {
		strcpy(curRoom, readRooms[i].nameArr[0]);
		posTemp = i;
		posCount = readRooms[i].curConns;
		}
	}

do {
	printf("CURRENT LOCATION: %s\n", curRoom);
	printf("POSSIBLE CONNECTIONS: ");
	for (i = 0; i < posCount; i++) {
		if (i == (posCount - 1)) {
			printf(" %s.\n", readRooms[posTemp].nameArr[i + 1]);
		}
		else if (i < posCount) {
			printf(" %s,", readRooms[posTemp].nameArr[i + 1]);
		}
	}
	printf("WHERE TO? >");
	do {
		isValid = false;
		char input[15];
		scanf("%s", input);
		printf("\n");
		for (i = 1; i <= posCount; i++) {
			if (strcmp(input, readRooms[posTemp].nameArr[i]) == 0) {
				strcpy(curRoom, readRooms[posTemp].nameArr[i]);
				stepCount++;
				pathTaken[pTakenCount] = malloc(15 * sizeof(char));
				strcpy(pathTaken[pTakenCount],curRoom);
				pTakenCount++;
				for (j = 0;j < 7;j++) {
					if (strcmp(readRooms[j].nameArr[0], curRoom) == 0) {
						posTemp = j;
						posCount = readRooms[j].curConns;
					}
				}
				strcpy(curType, readRooms[posTemp].typeArr);
				isValid = true;
			}
		}
		if (isValid == false) {
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
			printf("CURRENT LOCATION: %s\n", curRoom);
			printf("POSSIBLE CONNECTIONS: ");
			for (i = 0; i < posCount; i++) {
				if (i == (posCount - 1)) {
					printf(" %s.\n", readRooms[posTemp].nameArr[i + 1]);
				}
				else if (i < posCount) {
					printf(" %s,", readRooms[posTemp].nameArr[i + 1]);
				}
			}
			printf("WHERE TO? >");
		}
	} while (isValid == false);
} while (strcmp(curType, "END_ROOM") != 0);

printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCount);
for (i = 0; i < pTakenCount; i++) {
	printf("%s\n",pathTaken[i]);
}

for(i = 0; i < 7; i++) {			// free dynamically allocated memory
	free(rNames[i]);
	for (j = 0; j < 9; j++) {
		free(rooms[i].nameArr[j]);
		free(readRooms[i].nameArr[j]);
	}
}

exit(0);
}

/***********************************************
** Function that returns a random int within a specified range
**
************************************************/
int rangeRando(int floor, int ceiling){		

	if (g_rand == true) {
		srand(time(NULL));
		int rando = rand() % (ceiling + 1 - floor) + floor;
		g_rand = false;
		return rando;
	}
	else {
		srand(time(NULL));
		int rando = (rand() + g_val) % (ceiling + 1 - floor) + floor;
		g_val++;
		g_rand = true;
		return rando;
	}

}

/***********************************************
** Function that takse the struct Room array by reference and generates 7 random rooms for it
**
************************************************/
void generateRooms(struct Room * rooms) {

	int sortArr[7];
	int i, j, connCheck = 0;
	for (i = 0; i < 7; i++) {
		rooms[i].curConns = 0;
		for (j = 0; j < 9; j++) {  
			rooms[i].nameArr[j] = malloc(15 * sizeof(char));
			strcpy(rooms[i].nameArr[j], " ");
		}
	}
	for (i = 0; i < 7; i++) {

		int isSame = false;
		char name[15];
		do {
			isSame = false;
			int randNum = rangeRando(0, 6); 
			strcpy(name, rNames[randNum]);

			for (j = 0; j < 7; j++) {
				if (strcmp(name, rooms[j].nameArr[0]) == 0) {
					isSame = true;
				}
			}
			if (isSame == false) {
				strcpy(rooms[i].nameArr[0], name);
			}
		} while (isSame == true);
	}
		int isSame = false;
		int preSetStartEnd = rangeRando(0, 6);
		strcpy(rooms[preSetStartEnd].typeArr, "START_ROOM");
		do {																	// while loop ensures only one start and end room are printed
			isSame = false;
			preSetStartEnd = rangeRando(0, 6);
			if (strcmp(rooms[preSetStartEnd].typeArr, "START_ROOM") == 0) {
				isSame = true;
			}
			else {
				strcpy(rooms[preSetStartEnd].typeArr, "END_ROOM");
			}
		} while (isSame);
	
	isSame = false;
	int randNum = rangeRando(0, 2);
	for (j = 0; j < 7; j++) {																					// loop then assings the rest to mid rooms
		if ((strcmp(rooms[j].typeArr, "START_ROOM") == 0) || (strcmp(rooms[j].typeArr, "END_ROOM") == 0)) {
			continue;
		}
		else {
			strcpy(rooms[j].typeArr, "MID_ROOM");
		}
	}
	int totalConn = 0;
	int didConn = true;
	while(totalConn < 33) {		// loop randomly generates connections until total connects reaches a number that allows all rooms to have between 3 and 6 connections
		didConn = false;
		int randomNum = rangeRando(0, 6);
		if (rooms[randomNum].curConns < 7) {
			int randod = rangeRando(0, 6);
			int isPresent = false;
			char name[15];
			strcpy(name, rNames[randod]);
			if (strcmp(name, rooms[randomNum].nameArr[0]) != 0) {
				for (j = 1; j <= rooms[randomNum].curConns; j++) {			// checks if connection is already made
					if (strcmp(name, rooms[randomNum].nameArr[j]) == 0) {
						isPresent = true;
					}
				}
				if (isPresent != true) {			
					int k;
					for (k = 0; k < 7; k++) {
						if ((strcmp(rooms[k].nameArr[0], name) == 0) && (rooms[k].curConns < 7)) {
							int index = rooms[randomNum].curConns;
							strcpy(rooms[randomNum].nameArr[index + 1], name);
							index = rooms[k].curConns;
							strcpy(rooms[k].nameArr[index + 1], rooms[randomNum].nameArr[0]);
							rooms[randomNum].curConns++;
							rooms[k].curConns++;
							totalConn = totalConn + 2;
							didConn = true;
							continue;
						}
					}
				}
			}
		}
	}
}
