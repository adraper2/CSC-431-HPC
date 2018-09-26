#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 8
#define ITERATIONS 100000000

int global_inside[NUM_THREADS];

double distance(double x1, double y1, double x2, double y2) {
  return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}

void *monte_carlo_loop(void *thread_id){
  int tid = (int)(long) thread_id;

  int inside = 0;

  for (int i = 0; i < ITERATIONS/NUM_THREADS; i++) {
    // throw a random dart
    double x = arc4random_uniform(RAND_MAX)/(double)RAND_MAX;
    double y = arc4random_uniform(RAND_MAX)/(double)RAND_MAX;

    // is the dart inside the circle?
    double dist = distance(0, 0, x, y);
    if (dist < 1.0) {
      inside++;
    }
  }

  global_inside[tid] = inside;

  pthread_exit(NULL);
}

int main() {

  // start timing, struct is structure, timespec is variable type
  struct timespec start, end;
  //ambersand references object (structure) otherwise struct cannot change
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  pthread_t threads[NUM_THREADS];

  for (int t = 0; t < NUM_THREADS; t++){
    global_inside[t] = 0;
    int rc = pthread_create(&threads[t], NULL, monte_carlo_loop, (void *) (long) t);
    if(rc){ //any other than a 0 means it failed
      fprintf(stderr, "Oh no!\n");
      exit(-1);
    }
  }

  // wait for all threads to finish
  for (int t = 0; t < NUM_THREADS; t++){
    pthread_join(threads[t], NULL);
  }

  // sum array of inside values
  int sum = 0;
  for (int t = 0; t < NUM_THREADS; t++){
    sum += global_inside[t];
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &end);

  //compute ellapsed time (long long makes it 64-bit)
  unsigned long long delta_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
  printf("ellapsed time: %llu ms\n", delta_ms);

  printf("Iterations: %d ==> %g\n", ITERATIONS, sum/(double)ITERATIONS*4);

}
