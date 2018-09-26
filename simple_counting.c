#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 512

unsigned long count[NUM_THREADS];
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER; // mutual exclusion (only one thread in at a time)

void *increment_count(void *thread_id){
  int tid = (int)(long) thread_id;
  printf("thread %d is counting\n", tid);

  for(int i = 0; i <1000; i++){

    count[tid]++;
/*
    pthread_mutex_lock(&count_mutex);
    //this line is super important
    count = count + 1;
    pthread_mutex_unlock(&count_mutex);
*/
  }
  pthread_exit(NULL);
}

int main(){
  pthread_t threads[NUM_THREADS];

  for(int t = 0; t < NUM_THREADS; t++){
    count[t] = 0;
    int rc = pthread_create(&threads[t], NULL, increment_count, (void *) (long) t);
  }

  unsigned long full_count = 0;

  for(int t = 0; t < NUM_THREADS; t++){
    pthread_join(threads[t], 0);
    full_count += count[t];
  }
  printf("count: %lu\n", full_count);
}
