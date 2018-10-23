// Aidan Draper
// CSC 431 - Program 2: Collapse OpenMP

// use compile line:
/* gcc -o collapse_omp collapse_omp.c -fopenmp */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MY_FILE "10000000_ints_under_100000.txt"

// RESPONSE:
// Yes, it would be possible to perform more work within a thread than I have done below, but it does not seem computationally 
// intensive enough for it to make sense to additionally parallelize the integer collapses. There do not seem to be more than 
// 3-4 integer collapses, even for 10000000 integers. For that reason, it makes more sense to just collapse the array 
// because the overhead to parallelizing the integer collapses would probably add time to the total runtime even when
// scaling up. For this reason, it only makes sense to parallelize the array collapse using OpenMP. In this case, it 
// does make sense to use reduction. We could use OpenMP's critical function to define that "total" needs to use mutual
// exclusion during summattion, but it is much easier to just declare the variable and operation to that variable with
// the reduction line in a parrallel for loop. This lets the program know that "total" will be shared and addition will be
// performed on that variable across all threads. So yes, it does make sense to use it in this setting.

int main(){

	// variables
	int nums = 0; // integers in text file
	unsigned long long total = 0; // the sum of our array
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

	// dynamically initialize array of nums
    int * num_array = malloc(nums * sizeof(int));

    // scan integers into array from file
	for(int i = 0; i < nums; i++){
		//save to an array 
		fscanf(my_file, "%d", &num_array[i]);
	}

	fclose(my_file);

	// parallelize collapse of array
	#pragma omp parallel for shared(num_array) reduction(+: total)
		// calculate total from array
		for(int i = 0; i < nums; i++){
			total += num_array[i];
		}

	// collapse integer algorithm
	while(collapse != collapse % 10){
		collapse = 0; // reset collapse
		while(total){
    		collapse += total % 10;
    		total /= 10;
		}
		total = collapse;
	}

	printf("Our collapse: %d\n", collapse);


}


