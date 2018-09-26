// Aidan Draper
// CSC 431 - Program 1
// September 23, 2018

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
  if(argc != 2){ // error checking, were the right number of inputs supplied
    printf("Run with a single number integer, n, after program call\n");
    exit(-1);
  }
  int num = atoi(argv[1]); // convert input to number from char

  int steps = 0; // counter for the number of steps the input number takes to get to 1

  while(num != 1){ // while the input num does not equal 1, keep going
    if(num % 2 == 0){ // is it even
      num = num/2;
    } else { // then, it is odd
      num  = (num * 3) + 1;
    }
    steps += 1;
  }

  printf("The process took %d steps\n", steps);

}
