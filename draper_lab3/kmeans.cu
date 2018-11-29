// Aidan Draper
// CSC 431 - CUDA Kmeans Lab 3 Project
// November 28, 2018
// This is an application that parallelizes an original c file of a kmeans algorithm in CUDA.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <strings.h>
#include <time.h>
extern "C" { 
#include "gfx.h" 
}

typedef struct {
  int x;
  int y;
  int cluster;
} point_t;


typedef struct {
  point_t centroid;
  int size;
} set_t;

#define NO_CLUSTER -1
#define K 5

/* a way to choose colors */
unsigned int colors[] = { 0xFFFF00, 0x1CE6FF, 0xFF34FF, 0xFF4A46,
                          0x008941, 0x006FA6, 0xA30059, 0xFFDBE5, 0x7A4900,
                          0x0000A6, 0x63FFAC, 0xB79762, 0x004D43, 0x8FB0FF,
                          0x997D87, 0x5A0007, 0x809693, 0xFEFFE6, 0x1B4400,
                          0x4FC601, 0x3B5DFF, 0x4A3B53, 0xFF2F80, 0x61615A,
                          0xBA0900, 0x6B7900, 0x00C2A0, 0xFFAA92, 0xFF90C9,
                          0xB903AA, 0xD16100, 0xDDEFFF, 0x000035, 0x7B4F4B,
                          0xA1C299, 0x300018, 0x0AA6D8, 0x013349, 0x00846F,
                          0x372101, 0xFFB500, 0xC2FFED, 0xA079BF, 0xCC0744,
                          0xC0B9B2, 0xC2FF99, 0x001E09, 0x00489C, 0x6F0062,
                          0x0CBD66, 0xEEC3FF, 0x456D75, 0xB77B68, 0x7A87A1,
                          0x788D66, 0x885578, 0xFAD09F, 0xFF8A9A, 0xD157A0,
                          0xBEC459, 0x456648, 0x0086ED, 0x886F4C, 0x34362D,
                          0xB4A8BD, 0x00A6AA, 0x452C2C, 0x636375, 0xA3C8C9,
                          0xFF913F, 0x938A81, 0x575329, 0x00FECF, 0xB05B6F,
                          0x8CD0FF, 0x3B9700, 0x04F757, 0xC8A1A1, 0x1E6E00,
                          0x7900D7, 0xA77500, 0x6367A9, 0xA05837, 0x6B002C,
                          0x772600, 0xD790FF, 0x9B9700, 0x549E79, 0xFFF69F,
                          0x201625, 0x72418F, 0xBC23FF, 0x99ADC0, 0x3A2465,
                          0x922329, 0x5B4534, 0xFDE8DC, 0x404E55, 0x0089A3,
                          0xCB7E98, 0xA4E804, 0x324E72, 0x6A3A4C };

__global__ void assign(int num_observations, int size, point_t *dev_observations, set_t *dev_cluster){
     
	int tid = blockDim.x * blockIdx.x + threadIdx.x;
	if (tid < num_observations) {
		double min_mean = size * size;
		int min_cluster = NO_CLUSTER;
		for (int k = 0; k < K; k++) {
			double mean = powf(dev_observations[tid].x - dev_cluster[k].centroid.x, 2)
					+ powf(dev_observations[tid].y - dev_cluster[k].centroid.y, 2);
			if (min_mean > mean) {
				min_mean = mean;
				min_cluster = k;
			}
		}
		dev_observations[tid].cluster = min_cluster;
	}
}

/* draw the observations */
void show_observations(int num_observations, point_t *observation, set_t *cluster) {
   gfx_clear();

   // show the observations
   int j, i;
   for (j = 0; j < K; j++) {
      // change to the set color
      for (i = 0; i < num_observations; i++) {
         if (observation[i].cluster == -1) {
            gfx_color(255, 255, 255);
         } else {
            gfx_color((colors[observation[i].cluster] >> 16) & 0xFF,
                      (colors[observation[i].cluster] >> 8) & 0xFF,
                      colors[observation[i].cluster] & 0xFF);
         }
         gfx_line(observation[i].x, observation[i].y, observation[i].x, observation[i].y);
      }
   }

   for (i = 0; i < K; i++) {
      gfx_color((colors[i] >> 16) & 0xFF,
                (colors[i] >> 8) & 0xFF,
                colors[i] & 0xFF);

      gfx_line(cluster[i].centroid.x, cluster[i].centroid.y - 10, cluster[i].centroid.x, cluster[i].centroid.y + 10);
      gfx_line(cluster[i].centroid.x - 10, cluster[i].centroid.y, cluster[i].centroid.x + 10, cluster[i].centroid.y);
   }

   gfx_flush();
}

int main() {
	
  // only need to malloc structs and arrays

  /* read first line to determine how much data */
  int size, num_observations;
  scanf("%d %d", &num_observations, &size);

  /* data */
  point_t observations[num_observations];
  set_t cluster[K];

  /* read the data in */
  for (int i = 0; i < num_observations; i++) {
    scanf("%d %d", &(observations[i].x), &(observations[i].y));
    observations[i].cluster = NO_CLUSTER;
  }

  /* randomly set centroids */
  unsigned int seed = (unsigned int) time(NULL);
  for (int i = 0; i < K; i++) {
    int which = rand_r(&seed) % num_observations;

    // Forgy's Method
    cluster[i].centroid.x = observations[which].x;
    cluster[i].centroid.y = observations[which].y;

    cluster[i].size = 0;
    cluster[i].centroid.cluster = i;
  }
	
	point_t *dev_observations;
	set_t *dev_cluster;	

  // allocate memory and fill cluster and observation variables for CUDA
	cudaMalloc(&dev_observations, num_observations * sizeof(point_t));
	cudaMalloc(&dev_cluster,  K * sizeof(set_t));
	cudaMemcpy(dev_observations, &observations, num_observations * sizeof(point_t), cudaMemcpyHostToDevice);
	cudaMemcpy(dev_cluster, &cluster, K * sizeof(set_t), cudaMemcpyHostToDevice);

  gfx_open(size, size, "k-means clustering");
   
  set_t last_cluster[K]; // keeps track of our last cluster and centroids
  int centroid_move = 1; // a boolean value for checking convergence
  int iterations = 0;

  while (centroid_move > 0) {

    show_observations(num_observations, observations, cluster);

    // set all cluster sizes to 0
    for (int k = 0; k < K; k++) {
       cluster[k].size = 0;
    }

    iterations++;
    printf("Iteration %d\n", iterations);

    // Assignment step in cuda
    int thread_size = 256;
    int block_size = (num_observations + thread_size - 1)/ thread_size;
    
    assign<<<block_size,thread_size>>>(num_observations, size, dev_observations, dev_cluster);

    // Copy back down the assignment step
    cudaMemcpy(observations, dev_observations, num_observations * sizeof(point_t), cudaMemcpyDeviceToHost);
    cudaMemcpy(cluster, dev_cluster, K * sizeof(set_t), cudaMemcpyDeviceToHost);	

    // Update Step (calculate new centroids)
    if(iterations > 1){ // if its the second run or later, start recording the previous cluster
      memcpy(last_cluster, cluster, K * sizeof(set_t));
    }
    int sum_x[K], sum_y[K];
    bzero(sum_x, K * sizeof(int));
    bzero(sum_y, K * sizeof(int));

    //reset cluster size before checking it in the next loop
    for(int k = 0; k < K; k++){
      cluster[k].size = 0;
    }

    for (int i = 0; i < num_observations; i++) {
      cluster[observations[i].cluster].size++; // update size of each cluster as you go
      sum_x[observations[i].cluster] += observations[i].x; 
      sum_y[observations[i].cluster] += observations[i].y;
    }

    for (int k = 0; k < K; k++) { 
  printf("%d: sum_x = %d, sum_y = %d, cluster.size = %d\n", k, sum_x[k], sum_y[k], cluster[k].size);
       cluster[k].centroid.x = sum_x[k] / cluster[k].size;
       cluster[k].centroid.y = sum_y[k] / cluster[k].size;
       printf("cluster %d: (%d, %d)\n", k, cluster[k].centroid.x, cluster[k].centroid.y);
    }

  // Update CUDA's Versions of observations and cluster
  cudaMemcpy(dev_observations, &observations, num_observations * sizeof(point_t), cudaMemcpyHostToDevice);
  cudaMemcpy(dev_cluster, &cluster, K * sizeof(set_t), cudaMemcpyHostToDevice);

  // after first iteration, check for convergence
  if (iterations > 1){
    centroid_move = 0; // if all are same this will not change
    for (int k = 0; k < K; k++) {
      // if centroids have not changed, WE CONVERGED!
      if (last_cluster[k].centroid.x != cluster[k].centroid.x || last_cluster[k].centroid.y != cluster[k].centroid.y){
        centroid_move = 1;
      }
    }
  }

    sleep(1);
  }

  printf("Done with %d itertions\n", iterations);

  while (1) {
    char c = gfx_wait();
    if (c == 'q') break;
  }
}

// ssh -Y draper@10.52.1.13
/* gcc -o kmeans kmeans.c gfx.c -I/usr/X11/include -L/usr/X11/lib -lX11 -lm */
