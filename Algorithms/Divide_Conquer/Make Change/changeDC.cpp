#include <iostream>
#include <iomanip>
#include <chrono>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <algorithm>
#include <stdio.h>

const int NUM_OF_SETS = 3;

struct Result changeDC(int *V, int total, int length, int iVal);
void timeTest(int V[], int vlength, int A, FILE *output);

struct Result {
	int m = 0;
	int C[20] = { 0 };
};

struct TestVals {
	int A = 0;
	int V[20] = { 0 };
	int length = 0;
};
	/****************************************************************************************************/
int main() {

	struct TestVals set1;
	struct TestVals set2;
	struct TestVals set3;
	struct TestVals *holder[3];
	holder[0] = &set1;
	holder[1] = &set2;
	holder[2] = &set3;

	/****************************************************************************************************/
	// This section opens a file of test values and assigns them to a struct for program use.
	FILE *file;
	file = fopen("coins.txt", "r");
	char rSTok[15];
	char * tok;
	int ch;
	for (int i = 0; i < NUM_OF_SETS; i++){
		int j = 0;
		fgets(rSTok, 15, file);
		tok = strtok(rSTok, " :");
			while(tok != NULL){
			(*holder[i]).V[j] = atoi(tok);
			(*holder[i]).length++;
			j++;
			tok = strtok(NULL, " :");
			}
		fgets(rSTok, 15, file);
		tok = strtok(rSTok, " :");
		(*holder[i]).A = atoi(tok);
	}
	fclose(file);

	file = fopen("Amountchange.txt", "a");
	fprintf(file, "Algorithm changeslow\n");

	/****************************************************************************************************/
	// TEST 1: Coins.txt: Set 1
	timeTest(set1.V, set1.length, set1.A, file);
	/****************************************************************************************************/
	// TEST 2: Coins.txt: Set 2
	timeTest(set2.V, set2.length, set2.A, file);
	/****************************************************************************************************/
	// TEST 3: Coins.txt: Set 3
	//timeTest(set3.V, set3.length, set3.A, file);  // Takes SO LONG
	/****************************************************************************************************/
	// TEST 4: Test for Correctness
	int V4[] = { 1,2,4,8 };
	int total = 15;
	int vLength = sizeof(V4) / sizeof(int);
	timeTest(V4, vLength, total, file);
	/****************************************************************************************************/
	// TEST 5: Test for Correctness
	int V5[] = { 1,3,7,12 };
	total = 29;
	vLength = sizeof(V5) / sizeof(int);
	timeTest(V5, vLength, total, file);
	/****************************************************************************************************/
	// TEST 6: Test for Correctness
	int V6[] = { 1,3,7,12 };
	total = 31;
	vLength = sizeof(V6) / sizeof(int);
	timeTest(V6, vLength, total, file);
	/****************************************************************************************************/
	// Project Requirement #3: V = [1, 5, 10, 25, 50]
	/****************************************************************************************************/
	int V7[] = { 1, 5, 10, 25, 50 };
	total = 50;
	vLength = sizeof(V7) / sizeof(int);
	timeTest(V7, vLength, total, file);
	/***********************************/
	total = 55;
	timeTest(V7, vLength, total, file);
	/***********************************/
	total = 60;
	timeTest(V7, vLength, total, file);
	/***********************************/
	total = 65;
	timeTest(V7, vLength, total, file);

	/****************************************************************************************************/
	int V9[] = { 1, 6, 13, 37, 150 };
	total = 30;
	vLength = sizeof(V9) / sizeof(int);
	timeTest(V9, vLength, total, file);
	/****************************************************************************************************/
	total = 35;
	timeTest(V9, vLength, total, file);
	/****************************************************************************************************/
	total = 40;
	timeTest(V9, vLength, total, file);
	/****************************************************************************************************/
	total = 45;
	timeTest(V9, vLength, total, file);
	/****************************************************************************************************/
	total = 50;
	timeTest(V9, vLength, total, file);
	/****************************************************************************************************/
//	total = 55;
//	timeTest(V9, vLength, total, file);
	/****************************************************************************************************/
//	total = 60;
//	timeTest(V9, vLength, total, file);
	/****************************************************************************************************/
	// Project Requirement #5: V = [1, 2, 4, 6, 8, 10, 12, …, 30]
	/****************************************************************************************************/
	int V10[] = { 1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30 };
	total = 30;
	vLength = sizeof(V10) / sizeof(int);
	timeTest(V10, vLength, total, file);
	/****************************************************************************************************/
	total = 35;
	timeTest(V10, vLength, total, file);
/****************************************************************************************************/

	fclose(file);
	getchar();
	return 0;
}

struct Result changeDC(int *V, int k, int length, int iVal) {

	struct Result result;

	if (k < 0) {
		result.m = 10000;
		return result;
	}

	if (k == 0) {
		result.C[iVal]++;
		result.m = 0;
		return result;
	}

	struct Result recAr[20];
	for (int i = 0; i < length; i++)
	{
		recAr[i] = changeDC(V, k - V[i], length, i);
	}

	int minVal = 100000;
	int index = 0;
	for (int i = 0; i < length; i++) {
		if (recAr[i].m <= minVal) {
			minVal = recAr[i].m;
			index = i;
		}
	}
	recAr[index].m++;
	if (iVal >= 0) {
		recAr[index].C[iVal]++;
	}
	return recAr[index];
}

void timeTest(int V[], int vLength, int A, FILE *output ) {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;
	start = std::chrono::system_clock::now();
	struct Result answer = changeDC(V, A, vLength, -1);
	end = std::chrono::system_clock::now();
	elapsed_seconds = end - start;
	std::cout << elapsed_seconds.count() * 1000000 << std::endl;

	for (int i = 0; i < vLength; i++) {
		std::cout << V[i] << " ";
		fprintf(output, "%i ", V[i]);
	}
	std::cout << std::endl;
	fprintf(output, "\n");
	for (int i = 0; i < vLength; i++) {
		std::cout << answer.C[i] << " ";
		fprintf(output, "%i ", answer.C[i]);
	}
	fprintf(output, "\n");
	std::cout << std::endl;
	fprintf(output, "%i\n", answer.m);
	std::cout << answer.m << std::endl << std::endl;
}