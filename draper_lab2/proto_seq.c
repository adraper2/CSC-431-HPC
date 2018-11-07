// Aidan Draper 
// CSC 431 - Prototeins Lab 2
// Sequential version of a program that computes and scores self avoiding walks in prototeins.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void makeBase4(int base10, char *base4, int r);
void showWalk(char *walk, int r);
int doWalk(char *walk, char *proto, int r);
int checkSelfAvoid(int *past, int r);
int getScore(int *past, char *proto, int r);

// Parameters for Prototein walks
int overlaps = 0; // number of non-self avoiding walks
int maxScore = 0;

int main(int argc, char **argv){

	if (argc < 2){
		fprintf(stderr, "%s <proto>\n",argv[0]);
		exit(-1);
	}

	char *proto = argv[1];
	int r = strlen(proto);

	int maxWalks[(int)pow(4,r-1)]; // indices of max score walks
	int maxCount = 0;

	for(int base10=0; base10 < (int)pow(4,r-1); base10++){
		//printf("%d ",base10);
		char walk[r-1];
		makeBase4(base10, walk, r);
		//showWalk(walk, r);

		int score = doWalk(walk, proto, r);

		if (score > maxScore){ //replace the initial max score and restart indexing the walks for max score
			
			maxScore = score;
			for(int x = 0; x <= maxCount; x++){
				maxWalks[x] = 0;
			}
			maxCount = 0;
			maxWalks[maxCount] = base10;
			
		} else if (score == maxScore){ //add the index of this max score walk
			maxCount += 1;
			maxWalks[maxCount] = base10;
		}

	}

	printf("\nThe max score was %d at walks", maxScore);

	for (int i = 0; i <= maxCount; i++){
		printf(" %d",maxWalks[i]);
	}

	printf(".\n\nThere were %d self-avoiding walks.\n\n", ((int)pow(4,r-1)) - overlaps);


}

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
		overlaps += 1;
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



