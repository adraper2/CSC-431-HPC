// Aidan Draper 
// CSC 431 - Prototeins Lab 2
// Parallelized version of a program that computes and scores self avoiding walks in prototeins.

// compile with: mpicc -o proto_mpi proto_mpi.c
// run with: mpirun -np 4 ./proto_mpi HPHHHP

// For starters, this program is off by one highest-scored protein in the array. To improve the parallized version
// we may want to consider a better way to integrate walk appending. The overhead of comparing each processes results
// is sometimes greater than the computation needed for the sequential problem. This is usally at the smaller scales,
// but I am curious how the program could handle larger input and more processes where the root process would have to compare 
// every other process results to the current best. I think parallelizing the recieving of process results (so that not just my root process is working on this stuff)
// could be a way to improve the performance. The jobs for the other processes basically ends at that point and they could technically be doing 
// some of that work as well.

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <time.h>

void makeBase4(int base10, char *base4, int r);
void showWalk(char *walk, int r);
int doWalk(char *walk, char *proto, int r);
int checkSelfAvoid(int *past, int r);
int getScore(int *past, char *proto, int r);
void visualizeWalk(int walk, int r, char *proto);

#define NORMAL_TAG 11

struct timespec start, end;

int main(int argc, char **argv){

	if (argc < 2){
		fprintf(stderr, "%s <proto>\n",argv[0]);
		exit(-1);
	}

	// MPI Initialization: Setup ranks and number of procedures
	MPI_Init(&argc, &argv);

	int rank, size;
   	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   	MPI_Comm_size(MPI_COMM_WORLD, &size);

   	char *proto = argv[1]; // read in the string
	int r = strlen(proto); // compute length of the string

	// Parameters for Prototein walks
	//int overlaps = 0; // number of non-self avoiding walks
	int maxScore = 0;

   	int sublength = (int)pow(4,r-1) / size; // incremental size of walk array

	int maxWalks[sublength]; // indices of max score walks
	int maxCount = 0;

	if(rank == 0){
	// init time structures
  	clock_gettime(CLOCK_MONOTONIC_RAW, &start); // get first instance of time
	}

	// PARALLELIZE THIS (all ranks should do this but not for the entire loop)
	for(int base10=rank * sublength; base10 < (rank + 1) * sublength; base10++){
		//printf("%d\n",base10);
		char walk[r-1];
		makeBase4(base10, walk, r);
		//showWalk(walk, r);

		int score = doWalk(walk, proto, r);

		if (score > maxScore){ //replace the initial max score and restart indexing the walks for max score
			
			maxScore = score;
			for(int x = 0; x < maxCount; x++){
				maxWalks[x] = 0;
			}
			maxCount = 0;
			maxWalks[maxCount] = base10;
			
		} else if (score == maxScore){ //add the index of this max score walk
			maxWalks[maxCount+1] = base10;
			maxCount += 1;
		}

	}
	
	//printf("Rank %d produced %d\n",rank, maxCount);
	fflush(NULL);

	// send maxScore, maxWalks, and maxCount to rank 0
	if (rank != 0){

		MPI_Send(&maxCount, 1, MPI_INT, 0, NORMAL_TAG, MPI_COMM_WORLD);
		MPI_Send(maxWalks, maxCount, MPI_INT, 0, NORMAL_TAG, MPI_COMM_WORLD);
		MPI_Send(&maxScore, 1, MPI_INT, 0, NORMAL_TAG, MPI_COMM_WORLD);

	}
	
	MPI_Status status;
	MPI_Barrier(MPI_COMM_WORLD); // wait for other processes before recieving their data
	// recieves all data from other processes and find the best combined scores of all
	if (rank == 0){ // tell the master process how to compare max walks
		
		int best = maxScore; // this is the best max score that we currently have
		int bestCount = maxCount;
		int *bestWalks = (int *) malloc(sizeof(int)*(int)pow(4,r-1)); // this is our collection of highest scoring walks
      	for(int x = 0; x <=  bestCount; x++){
			bestWalks[x] = maxWalks[x];
		}


		//recieve scores for each and compares results to current best
      	for (int r = 1; r < size; r++) {
			MPI_Recv(&maxCount, 1, MPI_INT, r, NORMAL_TAG, MPI_COMM_WORLD, &status);
        	MPI_Recv(maxWalks, maxCount, MPI_INT, r, NORMAL_TAG, MPI_COMM_WORLD, &status);
        	MPI_Recv(&maxScore, 1, MPI_INT, r, NORMAL_TAG, MPI_COMM_WORLD, &status);

			if(maxScore > best){
        		best = maxScore;
        		for(int x = 0; x <= bestCount; x++){
					bestWalks[x] = 0; // first reset all of the best walks
				}
			for(int x=0; x <= maxCount; x++){
				bestWalks[x] = maxWalks[x];
			}
			bestCount = maxCount;
        	} else if (maxScore == best){
        		// append the walks to current best
				for(int x = bestCount+1; x < bestCount + maxCount+1; x++){
					
					bestWalks[x] = maxWalks[x-bestCount-1];

				} 
				bestCount = bestCount + maxCount;

        	}
      	}

      	clock_gettime(CLOCK_MONOTONIC_RAW, &end); // get second instance of time

  		// compute time in milliseconds and convert to ull 64 bit (from in-class example)
  		unsigned long long ellapsed = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;

  		printf("The parallel program took %llu ms\n", ellapsed);

     	printf("\nThe max score was %d at walks", best);

		for (int i = 0; i <= bestCount; i++){
			printf(" %d",bestWalks[i]);
		}

		printf(".\n\n Here are three of them for reference:\n\n");


		//print out 3 walks with ASCII (EXTRA PART)
		for (int i = 0; i < 3; i++){
			visualizeWalk(maxWalks[i], r, proto);
		}


   	} 
	printf("\n");
	MPI_Finalize();
}

// makes base 10 numbers base 10
void makeBase4(int base10, char *base4, int r){
	int num = base10;

	for(int p = r-2; p >= 0; p--){
		int div = (int)pow(4,p);
		base4[(r-2) -p] = (num/div) + '0';
		num %= div;
	}


}

void showWalk(char *walk, int r){

	for (int i = 0; i < r-1; i++){
		printf("%c", walk[i]);
	}

}

// does walk and checks for self avoidance and also scores it
int doWalk(char *walk, char *proto, int r){
	
	// cartesian plane coordinates 
	int plane_x = r; // set to max value to avoid negative numbers
	int plane_y = r;

	int past_xy [r];

	past_xy[r-1] = 100 * plane_x + plane_y; // starting x and y point

	// calculate next coordinate
	for (int i = r-2; i > -1; i--){

		if(walk[i] == '0'){ // North
			
			plane_x += 1;

		} else if(walk[i] == '1'){ // South

			plane_x -= 1;

		} else if(walk[i] == '2'){ // East

			plane_y += 1;

		} else if(walk[i] == '3'){ // West

			plane_y -= 1;

		} else{

			fprintf(stderr, "%c is an invalid walk number\n",walk[i]);
			exit(-1);
		}

		past_xy[i] = 100 * plane_x + plane_y; // concatenate two numbers in a unique, non-overlapping way
	}

	if (checkSelfAvoid(past_xy, r) != 1){ // if self avoids
		
		return getScore(past_xy,proto,r);

	} else { // does not self avoid
		return 0;
	}
}

int checkSelfAvoid(int *past, int r){
	for (int i = 0; i < r; i++){
		for (int j = i+1; j <= r-1; j++){
			if(past[i]==past[j]){
				//printf(" Prototein does not self avoid.\n");
				return 1;
			}
		}
	}
	//printf("\n");
	return 0;
}

// gets score of one walk
int getScore(int *past, char *proto, int r){

	int score = 0;

	for(int i = 0; i <= r; i++){
		for(int j = i+1; j <= r - 1; j++){
			if((abs(past[i]-past[j])==1 || abs(past[i]-past[j])==100) && proto[r-i-1] == 'H' && proto[r-j-1] == 'H'){
				score += 1;
			} 
		}
	}
	return score;
}

// ASCII visual of a single walk
void visualizeWalk(int walk, int r, char *proto){
	char base4[r-1];
	makeBase4(walk, base4, r);

	char gridXY[r*2][r*2];

	for(int i = 0; i < r*2; i++){
		for(int j = 0; j < r*2; j++){
			gridXY[i][j] = ' ';
		}
	}	


	gridXY[r][r] = 'H';

	int xPos = r;
	int yPos = r;

	for(int x = 0; x < strlen(base4); x++){
		if(base4[x] == '0'){ // North
			
			xPos += 1;
			gridXY[xPos][yPos] = proto[x];

		} else if(base4[x] == '1'){ // South

			xPos -= 1;
			gridXY[xPos][yPos] = proto[x];

		} else if(base4[x] == '2'){ // East

			yPos += 1;
			gridXY[xPos][yPos] = proto[x];

		} else if(base4[x] == '3'){ // West

			yPos -= 1;
			gridXY[xPos][yPos] = proto[x];
		}
	}

	for(int i = 0; i < r*2; i++){
		for(int j = 0; j < r*2; j++){
			printf("%c",gridXY[i][j]);
		}
		printf("\n");
	}	

	printf("\n");
	
	printf("That was for the walk with directions: %s\n", base4);
}



