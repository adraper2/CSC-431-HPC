// Aidan Draper
// CSC 431 Lab 1 - HEAT
// October 1st, 2018

// # of ms to find steady state (sequential)
// # of ms to find stead state (parallel, t = 2)
// # of ms to find stead state (parallel, t = 4)
// # of ms to find stead state (parallel, t = 8)
// room size = (800x800), ambient = 0.5,
// heater_size = 200, single heater center on (400, 400)

// I parallelized by row because

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "gfx.h"

#define SZ 800
#define AMBIENT 0.5
#define COLD 0.0
#define HOT 1.0
#define HEATER_COUNT 2
#define HEATER_SZ 200
#define ITERATIONS 10000
#define K 0.25
#define NUM_THREADS 4

// method declaration
void init_grid(void);
void init_heaters(void);
void *update_heat(void *thread_id);

// grids for heat program
double new_grid[SZ][SZ];
double old_grid[SZ][SZ];

// heater x and y parameter arrays
int heater_location_x[HEATER_COUNT];
int heater_location_y[HEATER_COUNT];

// starting point for each thread
int starting_x[NUM_THREADS];


int main(int argc, char **argv){
  if(argc != 2){ // error checking, were the right number of inputs supplied
    printf("Run with a single number integer, d, representing the delta threshold after the program call\n");
    exit(-1);
  }

  int delta = atof(argv[1]);

  for (int a = 0; a < NUM_THREADS; a++){
    starting_x[a] = (SZ / NUM_THREADS) * a; // remove from for loop
  }

  init_grid();
  init_heaters();

  gfx_open(SZ,SZ, "Heat Transfer pthreads");

  for(int i = 0; i<ITERATIONS; i++){

    // draw the room
    for (int x=0; x<SZ; x++){
      for (int y=0; y<SZ; y++){
        double p = old_grid[x][y];
        if(p>0.66){
          gfx_color(255*p,255*(1-p),0);
        } else if (p>0.33){
          gfx_color(255*p*1.5,255*p*1.5,0);
        } else {
          gfx_color(0,0,255*(1-p));
        }
        gfx_line(x,y,x,y);
      }
    }

    gfx_flush();

    pthread_t threads[NUM_THREADS];

    //parallelized update of heat cells
    for (int t = 0; t < NUM_THREADS; t++){
      int rc = pthread_create(&threads[t], NULL, update_heat, (void *) (long) t);
      if(rc){ //any other than a 0 means it failed
        fprintf(stderr, "Oh no! Threading error...\n");
        exit(-1);
      }
    }

    // wait for all threads to finish
    for (int t = 0; t < NUM_THREADS; t++){
      pthread_join(threads[t], NULL);
    }

    printf("Step %d\n", i); // print the step now that the threads have all finished

    // update the heaters
    for(int j = 0; j<HEATER_COUNT; j++){
      for(int x = 0; x < HEATER_SZ; x++){
        for(int y = 0; y < HEATER_SZ; y++){
          new_grid[heater_location_x[j] - HEATER_SZ/2 + x][heater_location_y[j] - HEATER_SZ/2 + y] = HOT;
        }
      }
    }

    //INSERT CHECK DELTA THRESHOLD

    // copy new grid to old grid
    for(int x = 0; x<SZ; x++){
      for(int y = 0; y<SZ; y++){
        old_grid[x][y] = new_grid[x][y];
      }
    }
  }

  while(1){ // end program by pressing q
    char c = gfx_wait();
    if(c == 'q') break;
  }

  return 0;
  pthread_exit(NULL);

}

void init_grid(){

  // initialize the grid
  for (int x=0; x<SZ; x++){
    for (int y=0; y<SZ; y++){
      old_grid[x][y] = AMBIENT;
      new_grid[x][y] = AMBIENT;

      // set edges to cold
      if(x == 0 || x == SZ-1 || y == 0 || y == SZ-1){
        old_grid[x][y] = COLD;
        new_grid[x][y] = COLD;
      }
    }
  }
}

void init_heaters(){

  // drop some heaters in the room
  srand(time(NULL)); // sets seed to current time

  for(int i = 0; i < HEATER_COUNT; i ++){
    // loation for heater
    int x_center = (int)(rand() % (SZ - 2 * HEATER_SZ)) + HEATER_SZ;
    int y_center = (int)(rand() % (SZ - 2 * HEATER_SZ)) + HEATER_SZ;

    // remember heater location
    heater_location_x[i] = x_center;
    heater_location_y[i] = y_center;

    for(int x = 0; x<HEATER_SZ; x++){
      for(int y = 0; y<HEATER_SZ; y++){
        old_grid[x_center - (HEATER_SZ/2) + x][y_center - (HEATER_SZ/2) + y] = HOT;
      }
    }
  }
}

void *update_heat (void *thread_id){
  int tid = (int)(long) thread_id;

  // I realize this is terrible code design but I could not figure out another way to catch for a bug that we worked through at the end of class.
  //This was the only way to seperate indexing so that the last grid was not indexing out of bounds.
  if((starting_x[tid]+ (SZ/NUM_THREADS))==SZ){
    for(int x = starting_x[tid]; x <  (starting_x[tid]+ (SZ/NUM_THREADS))-1; x++){
      for(int y = 1; y < SZ-1; y++){
        if(x!=-1){
          new_grid[x][y] = old_grid[x][y] + K * (old_grid[x+1][y] + old_grid[x][y+1] + old_grid[x-1][y] + old_grid[x][y-1] - 4 * old_grid[x][y]);
        }
      }
    }
  } else {
    for(int x = starting_x[tid]; x <  (starting_x[tid]+ (SZ/NUM_THREADS)); x++){
      for(int y = 1; y < SZ-1; y++){
        if(x!=-1){
          new_grid[x][y] = old_grid[x][y] + K * (old_grid[x+1][y] + old_grid[x][y+1] + old_grid[x-1][y] + old_grid[x][y-1] - 4 * old_grid[x][y]);
          if(new_grid[x][y]>1){
            printf("(%d,%d) has %f\n bc of values:\n%f\n%f\n%f\n%f\n",x,y,new_grid[x][y],old_grid[x+1][y], old_grid[x][y+1], old_grid[x-1][y], old_grid[x][y-1]);
            exit(-1);
          }
        }
      }
    }
  }
}
