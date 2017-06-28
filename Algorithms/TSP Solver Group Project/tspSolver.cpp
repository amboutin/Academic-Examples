//
//  tspSolver.cpp
//  CS325_Program4
//
//  Created by Edward Francis, Jessica Spokoyny, and Aaron Boutin
//  
//

#include "tspSolver.hpp"
#include <sstream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <chrono>
#include <ctime>

struct vertex{
    int vnum;
    int x;
    int y;
    int p;
    int key;
    int visited;
    int order;
};


int cityCount(char *infileName);
bool visited(int order[], int x, int numCities);
int distance(float x1, float y1, float x2, float y2);
int tourLength(int xCoord[], int yCoord[], int order[], int numCities);
int nearestNeighbor(int xCoord[], int yCoord[], int order[], int numCities);
int twoOpt(int xCoord[], int yCoord[], int order[], int numCities);

vertex** resizeArray(vertex** myArray, int* oldSize);
int ExtractMin(vertex** myArray, int size, int* remainingVertex);
int DFS(vertex** nodes, int** edges, int curNode, int* order, int numNodes);
void twoOptSwap(int* newRoute, int* route, int i, int k, int numNodes);
int getRouteLength(int* route, int** edges, int NumNodes);

int twoApprox(int argc, char* argv[], bool timelimit, bool optimize);
int christofides(int argc, char* argv[], bool timelimit, bool optimize);
int nearestn(int argc, char* argv[], bool timelimit, bool optimize);



//********************************************************************************
//Driver function
//********************************************************************************
int main(int argc, char* argv[]){
    
    // count lines
    int cityCount = 0;
    std::ifstream inFile;
    inFile.open(argv[1]);
    for (std::string line; getline(inFile, line); ++cityCount);
    std::cout << "\nNumber of cities: " << cityCount << std::endl;
    inFile.close();
    int cities = cityCount;
    bool timelimit = 0;
    
    if (argc > 2)
        timelimit = 1;
    
    if (cities == 76)
        christofides(argc, argv, timelimit, 1);
    else if (cities == 280)
        christofides(argc, argv, timelimit, 1);
    else if (cities == 15112)
        nearestn(argc, argv, timelimit, 0);
    else if (cities == 50)
        twoApprox(argc, argv, timelimit, 1);
    else if (cities == 100)
        christofides(argc, argv, timelimit, 1);
    else if (cities == 250)
    	christofides(argc, argv, timelimit, 1);
    else if (cities == 500)
        twoApprox(argc, argv, timelimit, 1);
    else if (cities == 1000){
        if(timelimit == true)
            nearestn(argc, argv, timelimit, 1);
        else
            twoApprox(argc, argv, timelimit, 1);
        }
    else if (cities == 2000){
        if(timelimit == true)
            nearestn(argc, argv, timelimit, 0);
        else
            twoApprox(argc, argv, timelimit, 1);
    }
    else if (cities == 5000){
        if(timelimit == true)
            twoApprox(argc, argv, timelimit, 1);
        else
            nearestn(argc, argv, timelimit, 0);
    }
        
    return 0;
}


//********************************************************************************
//2-approx algorithm
//********************************************************************************

int twoApprox(int argc, char* argv[], bool timelimit, bool optimize){
    
    std::cout << "Running Two-Approx with settings: timelimit: " << timelimit << ", optimize: " << optimize << std::endl;
    
    //import graph data
    //find MST of graph
    //double every edge to obtain new graph
    //find Eulerian circuit in new graph (visits every edge exactly once)
    //return tour that visits the vertices in the order of their first appearance in the Eulerian circuit
    
    //start clock
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds;
    
    //import graph data
    vertex** varray = new vertex*[50];
    int arraySize = 50;
    int arrayCount = 0;
    std::cout << "Importing graph data." << std::endl;
    std::ifstream f;
    

    f.open(argv[1]);
    std::string line;
    while(f){
        line = "";
        std::getline(f, line);
        if(line == "")
            break;
        if(arrayCount >= arraySize)
            varray = resizeArray(varray, &arraySize);
        vertex* newNode = new vertex;
        std::istringstream iss(line);
        iss >> newNode->vnum;
        iss >> newNode->x;
        iss >> newNode->y;
        newNode->visited = 0;
        newNode->key = 100000000;
        varray[arrayCount++] = newNode;
    }
    f.close();
    
    
//create connection matrix
std::cout << "Creating adjacency matrix." << std::endl;
int** edges = new int*[arrayCount];
for(int i = 0; i< arrayCount; i++){
    edges[i] = new int[arrayCount];
}

for(int i = 0; i < arrayCount; i++){
    for(int j = 0; j < arrayCount; j++){
        int xs = (varray[i]->x - varray[j]->x) * (varray[i]->x - varray[j]->x);
        int ys = (varray[i]->y - varray[j]->y) * (varray[i]->y - varray[j]->y);
        int index1 = varray[i]->vnum;
        int index2 = varray[j]->vnum;
        double root = sqrt(xs + ys);
        root = root + 0.5;
        edges[index1][index2] = int(root);}}

//create minimum spanning tree with Prim's algorithm
std::cout << "Creating MST." << std::endl;
int ** mst = new int*[arrayCount];
for(int i = 0; i< arrayCount; i++){
    mst[i] = new int[arrayCount];
}

for(int i = 0; i < arrayCount; i++)
for(int j = 0; j < arrayCount; j++)
mst[i][j] = -1;

std::cout << "Creating remaining vertex structure." << std::endl;
int* remainingVertex = new int[arrayCount];
for(int i = 0; i < arrayCount; i++)
remainingVertex[i] = i;

std::cout << "Running Prim's algorithm." << std::endl;
int assignedV = 0;
int startingV = 0;
varray[startingV]->key = 0;
varray[startingV]->p = startingV;
while(assignedV < arrayCount){
    int u = ExtractMin(varray, arrayCount, remainingVertex);
    remainingVertex[u] = -1;
    assignedV++;
    for(int i = 0; i < arrayCount; i++){
        if((remainingVertex[i] != -1) && (edges[u][i] < varray[i]->key)){
            varray[i]->p = u;
            varray[i]->key = edges[u][i];}
    }
}

std::cout << "Updating MST edge list." << std::endl;
for(int i = 0; i < arrayCount; i++){
    mst[i][varray[i]->p] = mst[varray[i]->p][i] = edges[varray[i]->p][i];}

//find Eulerian circuit and get cost
std::cout << "DFS to put the nodes in order, starting with 0." << std::endl;
int order = 0;
startingV = 0;
DFS(varray, mst, startingV, &order, arrayCount);
int* vertexList = new int[arrayCount];
for(int i = 0; i < arrayCount; i++){
    int index = varray[i]->order;
    vertexList[index] = i;}
int bestcost = getRouteLength(vertexList, edges, arrayCount);
std::cout << "First vertex cost: " << bestcost << std::endl;

//compare to circuits found using other vertices as starting points
std::cout << "Test other options for starting vertex." << std::endl;
int startingVert = 0;
for(int i = 0; i < arrayCount; i++){
    if(i % 10 == 0){
        end = std::chrono::system_clock::now();
        elapsed_seconds = end-start;
        if(elapsed_seconds.count() > 170 && timelimit == true){
            break;}}
    for(int j = 0; j < arrayCount; j++)
        varray[j]->visited = 0;
    int order = 0;
    startingV = i;
    DFS(varray, mst, startingV, &order, arrayCount);
    for(int i = 0; i < arrayCount; i++){
        int index = varray[i]->order;
        vertexList[index] = i;
    }
    int tourLength = getRouteLength(vertexList, edges, arrayCount);
    if(tourLength < bestcost){
        bestcost = tourLength;
        startingVert = i;
    }
}

//put nodes in order, using best vertex
std::cout << "DFS to put the nodes in best order." << std::endl;
order = 0;
for(int j = 0; j < arrayCount; j++)
varray[j]->visited = 0;
DFS(varray, mst, startingVert, &order, arrayCount);
for(int i = 0; i < arrayCount; i++){
    int index = varray[i]->order;
    vertexList[index] = i;}
std::cout << "Best vertex cost pre-2-opt: " << bestcost << std::endl;
std::cout << "Best vertex: " << startingVert << std::endl;

    if(optimize == true){
 //Use 2-opt to improve solution
 std::cout << "Applying 2-opt." << std::endl;
 int* newRoute = new int[arrayCount];
 int improved = 1;
 end = std::chrono::system_clock::now();
 elapsed_seconds = end-start;
 while(improved == 1){
     if(elapsed_seconds.count() > 170 && timelimit == true) break;
     improved = 0;
     int best_length = getRouteLength(vertexList, edges, arrayCount);
     for(int i = 1; i < arrayCount - 2; i++){
         for(int k = i + 1; k < arrayCount -1; k++){
             if(k % 10 == 0){
             end = std::chrono::system_clock::now();
             elapsed_seconds = end-start;
             if(elapsed_seconds.count() > 170 && timelimit == true){
             break;}}
             twoOptSwap(newRoute, vertexList, i, k, arrayCount);
             if(getRouteLength(newRoute, edges, arrayCount) < best_length){
             int* temp = vertexList;
             vertexList = newRoute;
             newRoute = temp;
             improved = 1;
 		}
 	}
 if(elapsed_seconds.count() > 170 && timelimit == true)
 break;
 }
 end = std::chrono::system_clock::now();
 elapsed_seconds = end-start;
 }
}
 
//output results to screen and file
std::cout << "Printing results." << std::endl;
std::cout << "Total cost: " << getRouteLength(vertexList, edges, arrayCount) << std::endl;
std::cout << "Running time: " << elapsed_seconds.count() << std::endl;

std::ofstream of;
std::string ofstring = std::string(argv[1]) + ".tour";
of.open(ofstring);
of << getRouteLength(vertexList, edges, arrayCount) << std::endl;
for(int i = 0; i < arrayCount; i++)
of << vertexList[i] << std::endl;

of.close();

//clean up and exit
for (int i = 0; i < arrayCount; i++)
delete [] edges[i];
delete [] edges;
for (int i = 0; i < arrayCount; i++)
delete [] mst[i];
delete [] mst;
delete [] vertexList;
delete [] remainingVertex;
//  delete [] newRoute;
return 0;
}

//********************************************************************************
//Christofides algorithm
//********************************************************************************
int christofides(int argc, char* argv[], bool timelimit, bool optimize){
    
std::cout << "Running Christofides with settings: timelimit: " << timelimit << ", optimize: " << optimize << std::endl;

//import graph data
//find MST of graph
//double every edge to obtain new graph
//find Eulerian circuit in new graph (visits every edge exactly once)
//return tour that visits the vertices in the order of their first appearance in the Eulerian circuit

//start clock

std::chrono::time_point<std::chrono::system_clock> start, end;
start = std::chrono::system_clock::now();
std::chrono::duration<double> elapsed_seconds;

//import graph data
vertex** varray = new vertex*[50];
int arraySize = 50;
int arrayCount = 0;
std::cout << "Importing graph data." << std::endl;
std::ifstream f;

f.open(argv[1]);
std::string line;
while(f){
    line = "";
    std::getline(f, line);
    if(line == "")
        break;
    if(arrayCount >= arraySize)
        varray = resizeArray(varray, &arraySize);
    vertex* newNode = new vertex;
    std::istringstream iss(line);
    iss >> newNode->vnum;
    iss >> newNode->x;
    iss >> newNode->y;
    newNode->visited = 0;
    newNode->key = 100000000;
    varray[arrayCount++] = newNode;
}
f.close();
    
 //create connection matrix
 std::cout << "Creating adjacency matrix." << std::endl;
 int** edges = new int*[arrayCount];
 for(int i = 0; i< arrayCount; i++){
 edges[i] = new int[arrayCount];
 }
                
 for(int i = 0; i < arrayCount; i++){
    for(int j = 0; j < arrayCount; j++){
      int xs = (varray[i]->x - varray[j]->x) * (varray[i]->x - varray[j]->x);
      int ys = (varray[i]->y - varray[j]->y) * (varray[i]->y - varray[j]->y);
      int index1 = varray[i]->vnum;
      int index2 = varray[j]->vnum;
      double root = sqrt(xs + ys);
      root = root + 0.5;
      edges[index1][index2] = int(root);}}
                
    //create minimum spanning tree with Prim's algorithm
    std::cout << "Creating MST." << std::endl;
    int ** mst = new int*[arrayCount];
    for(int i = 0; i< arrayCount; i++){
        mst[i] = new int[arrayCount];
    }
    
    for(int i = 0; i < arrayCount; i++)
    for(int j = 0; j < arrayCount; j++)
    mst[i][j] = -1;
    
    std::cout << "Creating remaining vertex structure." << std::endl;
    int* remainingVertex = new int[arrayCount];
    for(int i = 0; i < arrayCount; i++)
    remainingVertex[i] = i;
    
    std::cout << "Running Prim's algorithm." << std::endl;
    int assignedV = 0;
    int startingV = 0;
    varray[startingV]->key = 0;
    varray[startingV]->p = 0;
    while(assignedV < arrayCount){
        int u = ExtractMin(varray, arrayCount, remainingVertex);
        remainingVertex[u] = -1;
        assignedV++;
        for(int i = 0; i < arrayCount; i++){
            if((remainingVertex[i] != -1) && (edges[u][i] < varray[i]->key)){
                varray[i]->p = u;
                varray[i]->key = edges[u][i];}
        }
    }
    
    std::cout << "Updating MST edge list." << std::endl;
    for(int i = 0; i < arrayCount; i++){
        mst[i][varray[i]->p] = mst[varray[i]->p][i] = edges[varray[i]->p][i];
        //std::cout << i << " : " << varray[i]->p << " : " << edges[i][varray[i]->p] << std::endl;
    }
    
    std::cout << "Finding odd-degree vertices." << std::endl;
    int* oddVertexes = new int[arrayCount];
    int oddcount = 0;
    for(int i = 0; i < arrayCount; i++){
        int edgecount = 0;
        for(int j = 0; j < arrayCount; j++)
            if ((mst[i][j] != -1) && (i != j))
                edgecount++;
        if(edgecount % 2 == 1){
            oddVertexes[oddcount] = i;
            oddcount++;}}
    
    std::cout << "Updating MST adjacency matrix with greedy-perfect matchings of odd vertices." << std::endl;
    while(oddcount > 0){
        int currentOdd = oddVertexes[oddcount - 1];
        int bestmatch = oddVertexes[oddcount - 2];
        int bestindex = oddcount - 2;
        int distance = edges[currentOdd][bestmatch];
        for(int i = 0; i < oddcount - 2; i++)
            if(edges[currentOdd][oddVertexes[i]] < distance){
                distance = edges[currentOdd][oddVertexes[i]];
                bestmatch = oddVertexes[i];
                bestindex = i;}
        mst[currentOdd][bestmatch] = mst[bestmatch][currentOdd] = edges[currentOdd][bestmatch];
        for(int i = bestindex; i < oddcount - 2; i++)
            oddVertexes[i] = oddVertexes[i+1];
        oddcount -= 2;
    }
    
    //find Eulerian circuit and get cost
    std::cout << "DFS to put the nodes in order, starting with 0." << std::endl;
    int* vertexList = new int[arrayCount];
    int order = 0;
    startingV = 0;
    DFS(varray, mst, startingV, &order, arrayCount);
    for(int i = 0; i < arrayCount; i++){
        int index = varray[i]->order;
        vertexList[index] = i;}
    int bestcost = getRouteLength(vertexList, edges, arrayCount);
    std::cout << "First vertex cost: " << bestcost << std::endl;
    
    
    //compare to circuits found using other vertices as starting points
    std::cout << "Test other options for starting vertex." << std::endl;
    int startingVert = 0;
    for(int i = 0; i < arrayCount; i++){
        if(i % 10 == 0){
            end = std::chrono::system_clock::now();
            elapsed_seconds = end-start;
            if(elapsed_seconds.count() > 170 && timelimit == true){
                break;}}
        for(int j = 0; j < arrayCount; j++)
            varray[j]->visited = 0;
        int order = 0;
        startingV = i;
        DFS(varray, mst, startingV, &order, arrayCount);
        for(int i = 0; i < arrayCount; i++){
            int index = varray[i]->order;
            vertexList[index] = i;
        }
        int tourLength = getRouteLength(vertexList, edges, arrayCount);
        if(tourLength < bestcost){
            bestcost = tourLength;
            startingVert = i;
        }
    }
    
    //put nodes in order, using best vertex
    std::cout << "DFS to put the nodes in best order." << std::endl;
    order = 0;
    for(int j = 0; j < arrayCount; j++)
    varray[j]->visited = 0;
    DFS(varray, mst, startingVert, &order, arrayCount);
    for(int i = 0; i < arrayCount; i++){
        int index = varray[i]->order;
        vertexList[index] = i;
    }
    std::cout << "Best vertex cost pre-2-opt: " << bestcost << std::endl;
    std::cout << "Best vertex: " << startingVert << std::endl;
    
    
    if(optimize == true){
        //Use 2-opt to improve solution
        std::cout << "Applying 2-opt." << std::endl;
        int* newRoute = new int[arrayCount];
        int improved = 1;
        end = std::chrono::system_clock::now();
        elapsed_seconds = end-start;
        while(improved == 1){
            improved = 0;
            if(elapsed_seconds.count() > 170 && timelimit == true) break;
            int best_length = getRouteLength(vertexList, edges, arrayCount);
            for(int i = 1; i < arrayCount - 2; i++){
                for(int k = i + 1; k < arrayCount -1; k++){
                    if(k % 10 == 0){
                        end = std::chrono::system_clock::now();
                        elapsed_seconds = end-start;
                        if(elapsed_seconds.count() > 170 && timelimit == true){
                            break;}}
                    twoOptSwap(newRoute, vertexList, i, k, arrayCount);
                    if(getRouteLength(newRoute, edges, arrayCount) < best_length){
                        int* temp = vertexList;
                        vertexList = newRoute;
                        newRoute = temp;
                        improved = 1;
                    }
                }
                if(elapsed_seconds.count() > 170 && timelimit == true)
                    break;
            }
            end = std::chrono::system_clock::now();
            elapsed_seconds = end-start;
        }
    }
    
    //output results to screen and file
    std::cout << "Printing results." << std::endl;
    std::cout << "Total cost: " << getRouteLength(vertexList, edges, arrayCount) << std::endl;
    std::cout << "Running time: " << elapsed_seconds.count() << std::endl;
    
    std::ofstream of;
    std::string ofstring = std::string(argv[1]) + ".tour";
    of.open(ofstring);
    of << getRouteLength(vertexList, edges, arrayCount) << std::endl;
    for(int i = 0; i < arrayCount; i++)
    of << vertexList[i] << std::endl;
    
    of.close();
    
    //clean up and exit
    std::cout << "Cleaning up." << std::endl;
    for(int i = 0; i < arrayCount; i++)
    delete varray[i];
    delete [] varray;
    for (int i = 0; i < arrayCount; i++)
    delete [] edges[i];
    delete [] edges;
    for (int i = 0; i < arrayCount; i++)
    delete [] mst[i];
    delete [] mst;
    delete [] vertexList;
    delete [] remainingVertex;
    //  delete [] newRoute;
    //    delete [] MSTedges;
    
    return 0;
    }

//********************************************************************************
//2-approx - Resize an array of vertexes
//********************************************************************************
vertex** resizeArray(vertex** myArray, int* oldSize){
    vertex** temp = new vertex*[*oldSize*2];
    for(int i = 0; i< *oldSize; i++){
        temp[i] = myArray[i];}
    delete [] myArray;
    myArray = temp;
    *oldSize *= 2;
    return myArray;
}

//********************************************************************************
//2-approx - Get lowest cost remaining vertex
//********************************************************************************
int ExtractMin(vertex** myArray, int arraysize, int* remainingVertex){
    int i = 0;
    while(remainingVertex[i] == -1)
        i++;
    int min = i;
    for(int i = min; i < arraysize; i++){
        if(remainingVertex[i] != -1){
            if(myArray[i]->key < myArray[min]->key)
                min = i;}}
    return min;
}

//********************************************************************************
//2-approx - DFS to order vertices
//********************************************************************************
int DFS(vertex** nodes, int** edges, int curNode, int* order, int numNodes){
    nodes[curNode]->visited = 1;
    nodes[curNode]->order = *order;
    for(int i = numNodes - 1; i >= 0; i--){
        if(edges[curNode][i] != -1)
            if(nodes[i]->visited != 1){
                (*order)++;
                DFS(nodes, edges, i, order, numNodes);}}
    return 0;
}

//********************************************************************************
//2-approx - 2-opt to improve solution
//********************************************************************************
void twoOptSwap(int* newRoute, int* route, int i, int k, int numNodes){
    for(int m = 0; m < i; m++)
        newRoute[m] = route[m];
    for(int m = i; m <= k; m++)
        newRoute[m] = route[i+k-m];
    for(int m = k+1; m < numNodes; m++)
        newRoute[m] = route[m];
    return;
}

//********************************************************************************
//2-approx - get route length
//********************************************************************************
int getRouteLength(int* route, int** edges, int NumNodes){
    int cost = 0;
    for(int i = 0; i < NumNodes - 1; i++)
        cost+= edges[route[i]][route[i+1]];
    cost+= edges[route[0]][route[NumNodes - 1]];
    return cost;
}

//********************************************************************************
//Nearest Neighbor algorithm
//********************************************************************************
int nearestn(int argc, char* argv[], bool timelimit, bool optimize){
    std::cout << "Running Nearest Neighbor with settings: timelimit: " << timelimit << ", optimize: " << optimize << std::endl;
    // find number of cities from input
    
    std::ifstream inFile;
    std::ofstream outFile;
    //start clock
    
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds;
    
    
    int cityCount = 0;
    // in/outFile
    inFile.open(argv[1]);
    std::string outfileName = argv[1];
    outfileName += ".tour";
    outFile.open(outfileName);
    
    // count lines
    for (std::string line; getline(inFile, line); ++cityCount);
    std::cout << "\nNumber of cities: " << cityCount << std::endl;
    inFile.clear();
    inFile.seekg(0, std::ios::beg);
    
    int numCities = cityCount;
    
    // just run nearest neighbor or also 2-opt - determine from bool optimize

    // set up and fill arrays
   std::string * cities = new std::string[numCities];
   int * xCoord = new int[numCities];
   int * yCoord = new int[numCities];
   int * order = new int[numCities];
   
   for (int i = 0; i < numCities; i++) {
       order[i] = -1;
   }
   
   std::string str;
   int k = 0;
   
   while (k < numCities) {
       // city name
       inFile >> str;
       cities[k] = str;
       
       // x coordinate
       inFile >> str;
       xCoord[k] = stoi(str);
       
       // y coordinate
       inFile >> str;
       yCoord[k] = stoi(str);
       
       k++;
   }
   
   int totalDist = nearestNeighbor(xCoord, yCoord, order, numCities);
   int tempDist;
   if (optimize == true) {
       do {
           elapsed_seconds = end-start;
           if(elapsed_seconds.count() > 170 && timelimit == true){ //if timed and limit reached, wrap up
               break;}
           tempDist = totalDist;
           totalDist = twoOpt(xCoord, yCoord, order, numCities);
           end = std::chrono::system_clock::now();
       } while (tempDist != totalDist);
   }

    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    //output results to screen and file
    std::cout << "Printing results." << std::endl;
    std::cout << "Total cost: " << totalDist << std::endl;
    std::cout << "Running time: " << elapsed_seconds.count() << std::endl;

    // output tour length and order to '.tour' file
    outFile << totalDist;
    for (int i = 0; i < numCities; i++) {
        std::string identifier = cities[order[i]];
        outFile << std::endl << identifier;
       }
    delete [] cities;
    delete [] xCoord;
    delete [] yCoord;
    delete [] order;
    inFile.close();
    outFile.close();
    return 0;
}

//********************************************************************************
//Nearest Neighbor algorithm - check visited
//********************************************************************************
           
// check if city has been visited
bool visited(int order[], int x, int numCities){
   for (int i = 0; i < numCities; i++) {
       if (order[i] == x)
           return true;
   }
   return false;
}

//********************************************************************************
//Nearest Neighbor algorithm - distance between cities
//********************************************************************************
// find distance between two cities
int distance(float x1, float y1, float x2, float y2) {
   return (int) round(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)));
}

//********************************************************************************
//Nearest Neighbor algorithm - length of tour
//********************************************************************************
// find length of tour
int tourLength(int xCoord[], int yCoord[], int order[], int numCities) {
   int length = 0;
   
   for (int i = 0; i < numCities - 1; i++) {
       length += distance(xCoord[order[i]], yCoord[order[i]], xCoord[order[i+1]], yCoord[order[i+1]]);
   }
   
   length += distance(xCoord[order[numCities - 1]], yCoord[order[numCities - 1]], xCoord[order[0]], yCoord[order[0]]);
   
   return length;
}

//********************************************************************************
//Nearest Neighbor algorithm - main algo
//********************************************************************************
           
// nearest neighbor
int nearestNeighbor(int xCoord[], int yCoord[], int order[], int numCities) {
   int length = 0;
   int current;
   int bestPoint = 0;
   int bestTourLength;
   int newTourLength;
   bool firstDist;
   
   // compare each city to all remaining unvisited cities - add closest city to tour
   for (int i = 0; i < numCities; i++) {
       current = bestPoint;
       order[i] = current;
       firstDist = true;
       bestTourLength = 0;
       for (int j = 0; j < numCities; j++) {
           if (visited(order, j, numCities))
               continue;
           
           newTourLength = distance(xCoord[current], yCoord[current], xCoord[j], yCoord[j]);
           
           if (newTourLength < bestTourLength || firstDist) {
               bestTourLength = newTourLength;
               bestPoint = j;
               firstDist = false;
           }
       }
       length += bestTourLength;
   }
   
   // add distance from last city visited back to starting city
   length += distance(xCoord[order[numCities - 1]], yCoord[order[numCities - 1]], xCoord[order[0]], yCoord[order[0]]);
   
   std::cout << "Nearest neighbor tour length: " << length << std::endl;
   
   return length;
}

//********************************************************************************
//Nearest Neighbor algorithm - 2-opt
//********************************************************************************
// 2-opt
int twoOpt(int xCoord[], int yCoord[], int order[], int numCities) {
   int length = 0;
   int idx;
   int bestTourLength;
   int newTourLength;
   bool improved;
   
   for (int i = 0; i < numCities - 1; i++) {
       // set best to current tour length
       bestTourLength = tourLength(xCoord, yCoord, order, numCities);
       idx = i;
       
       for (int j = i+1; j < numCities; j++) {
           // find tour length if i and j are swapped
           std::swap(order[i], order[j]);
           newTourLength = tourLength(xCoord, yCoord, order, numCities);
           
           std::swap(order[i], order[j]);
           
           // if new tour is shorter, store j and new length
           if (newTourLength < bestTourLength) {
               bestTourLength = newTourLength;
               idx = j;
           }
       }
       
       // swap points if shorter tour was found
       if (idx != i) {
           std::swap(order[i], order[idx]);
           improved = true;
       }
   }
   
   length = tourLength(xCoord, yCoord, order, numCities);
   std::cout << "\tImproved tour length: " << length << std::endl;
   
   return length;
}

           
           
           
           
           
           
