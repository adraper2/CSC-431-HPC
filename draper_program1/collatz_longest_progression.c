// Aidan Draper
// CSC 431 - Program 1
// September 23, 2018

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv){

  // init time structures
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start); // get first instance of time

  if(argc != 2){ // error checking, were the right number of inputs supplied
    printf("Run with a single number integer, n, after program call\n");
    exit(-1);
  }

  int num = atoi(argv[1]);
  int steps = 0;
  int lp = 0;

  // for all the numbers from 1 to input num
  for(int i = 1; i <= num; i++){

    int current = i;
    int i_steps = 0;

    // while i is still not 1, keep going
    while(current != 1){
      if(current % 2 == 0){
        current = current/2;
      } else {
        current  = (current * 3) + 1;
      }
      i_steps += 1;
    }
    if(i_steps > steps){ // set new variables if this instance took longer
      steps = i_steps;
      lp = i;
    }
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &end); // get second instance of time

  // compute time in milliseconds and convert to ull 64 bit (from in-class example)
  unsigned long long ellapsed = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;

  printf("The process took %d steps for %d\n", steps, lp);
  printf("It took %llu ms\n", ellapsed);

}
