#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_VALUE 10000

int main() {

  //int array[MAX_VALUE+1]; //static allocation

  //sizeof is how many bytes an integers is depending on the computer (4 is the iMacs)
  //malloc returns a void pointer; (int *) casts it to an integer pointer, you can make that char, float, etc.
  int * value = (int *)malloc(sizeof(int) * (MAX_VALUE + 1));  //dynamic allocation

  //set entire array to TRUE
  for(int i = 0; i <= MAX_VALUE; i++){
    value[i]=1; //any other number than 0
  }

  //start with 2
  int next_prime = 2;

  while(next_prime < sqrt(MAX_VALUE)){ //only have to go to square root of max value to check everything

    //mark the multiples of the prime number
    for(int i = next_prime * 2; i<=MAX_VALUE; i+=next_prime){ //compute next value as part of for loop
      value[i] = 0;
    }

    //find the next next_prime
    next_prime++;

    while(value[next_prime]!= 1 && next_prime<sqrt(MAX_VALUE)){ //searching for a 1 (the next prime)
      next_prime++;
    }

  }

  //display primes
  int count = 0;
  for (int x = 2; x <= MAX_VALUE; x++){
    if(value[x]==1){
      printf("%d\n", x);
      count++;
    }
  }
  printf("Found %i primes\n",count);

}
