#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NARC_NUM 20

int countDigits(int orig){
  int digits = 0;
  int now = orig;
  while(now != 0){
    now = now / 10;
    digits ++;
  }
  return digits;
}

int isNarc(int orig, int num_digits){
  int my_num = orig;
  int my_sum = 0;
  while(my_num != 0){
    my_num = my_num / 10;
    my_sum += my_sum + pow(my_num, num_digits);
  }
  printf("Original: %i my Sum: %i\n",orig, my_sum);
  if (orig - my_sum == 0){
    return 1;
  }
  return 0;
}

int main(){

  int matches = 0;
  int numb = 1;

  while(matches < NARC_NUM){
    int my_digits =  countDigits(numb);
    int maybe = isNarc(numb, my_digits);
    matches = matches + maybe;
    if (maybe - 1 == 0){
      printf("%i\n", numb);
    }
    numb++;
  }

}
