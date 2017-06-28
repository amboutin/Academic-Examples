The following program was written as a group effort to solve the Traveling Salesman Problem by close approximation.

#Students: Edward Francis, Jessica Spokoyny, and Aaron Boutin
#Class: CS 325 Algorithms
#Date: March 14, 2017
#Description: README File for Program 4: Travelling Salesperson Problem

This program can be used to generate a close-to-optimal solution for each of the TSP test cases provided for this assignment (tsp_example_[1...3].txt, test-input-[1...7].txt.

To run the program:

1) Compile it by executing the command "make" or "make all" or "make project4" "while in a directory containing the files tspSolver.cpp and tspSolver.hpp. The resulting executable will be called "tsp."

2) Ensure that the city list input files are in the same directory. The program will produce tour output files with the name "[INPUT FILENAME].tour". The program expects to use the same files that were provided for this assignment.

3) Run with the command "./tsp (INPUT FILENAME) [1]". The "1" is optional, and can be used to toggle a time limit to ensure that the program will run for less than 3 minutes.


For example, to run the program with the input file "tsp_example_1.txt" with no time limit, run this command:

./tsp tsp_example_1.txt

To run the program with the same input file, with a 3-minute time limit, run this command:

./tsp tsp_example_1.txt 1
