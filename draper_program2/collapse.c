// Aidan Draper
// CSC 431 - Program 2: Collapse Sequential
#include <stdio.h>
#include <stdlib.h>

#define MY_FILE "1000_ints_under_10000.txt"

int main(){

	// variables
	int nums = 0; // integers in text file
	int total = 0; // the sum of our array
	int collapse = 11; // our temp sum of our total after collapse (started at 11 to initalize first while loop)
	int num_collapse = 0; //track number of times we collapse

	FILE *my_file;

	my_file = fopen(MY_FILE, "r");

	// error catching
    if (my_file == NULL){
        printf("Error Reading File\n");
        exit (0);
    }

    fscanf(my_file, "%d", &nums);
    printf("My nums: %d\n", nums);

    int num_array[nums]; // the initial array of nums

    // scan integers into array from file
	for(int i = 0; i < nums; i++){
		//save to an array 
		fscanf(my_file, "%d", &num_array[i]);
		printf("%d\n", num_array[i]);
	}

	fclose(my_file);

	// calculate total from array
	for(int i = 0; i < nums; i++){
		total += num_array[i];
	}

	printf("Total: %d\n", total);

	// collapse algorithm sequential
	while(collapse != collapse % 10){
		collapse = 0; // reset collapse
		while(total){
    		collapse += total % 10;
    		total /= 10;
		}
		printf("Our collapse: %d\n", collapse);
		total = collapse;
	}

}


