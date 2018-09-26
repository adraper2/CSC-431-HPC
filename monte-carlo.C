#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ITERATIONS 10000000

double distance(double x1, double y1, double x2, double y2) {
  return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}

int main() {

  // start timing, struct is structure, timespec is variable type
  struct timespec start, end;
  //ambersand references object (structure) otherwise struct cannot change
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  int inside = 0;

  for (int i = 0; i < ITERATIONS; i++) {
    // throw a random dart
    double x = arc4random_uniform(RAND_MAX)/(double)RAND_MAX;
    double y = arc4random_uniform(RAND_MAX)/(double)RAND_MAX;

    // is the dart inside the circle?
    double dist = distance(0, 0, x, y);
    if (dist < 1.0) {
      inside++;
    }
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &end);

  //compute ellapsed time (long long makes it 64-bit)
  unsigned long long delta_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
  printf("ellapsed time: %llu ms\n", delta_ms);

  printf("Iterations: %d ==> %g\n", ITERATIONS, inside/(double)ITERATIONS*4);

}
