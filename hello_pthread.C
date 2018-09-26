#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 8

void *print_hello(void *thread_id){
  int tid = (int)(long) thread_id; //turn it back into an integer
  printf("Hello, world! It's me, thread %d\n", tid);
  fflush(NULL);
  pthread_exit(NULL);
}

int main(){
  pthread_t threads[NUM_THREADS]; //this is an array "int array[12];"

  for(int thr = 0; thr < NUM_THREADS; thr++){

    printf("In main: creating threads %d\n", thr);

    int rc = pthread_create(&threads[thr], NULL, print_hello, (void *)(long) thr); //turn 32 bit val into 64 into address

    if (rc) {
      fprintf(stderr, "Error: return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
}
