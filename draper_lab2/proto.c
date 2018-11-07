#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void makeBase4(int base10, char *base4, int r);
void showWalk(char *walk, int r);

int main(int argc, char **argv){

	if (argc < 2){
		fprintf(stderr, "%s <proto>\n",argv[0]);
		exit(-1);
	}

	char *proto = argv[1];
	int r = strlen(proto);

	for(int base10=0; base10 < (int)pow(4,r-1); base10++){
		printf("%d ",base10);
		char walk[r-1];
		makeBase4(base10, walk, r);
		showWalk(walk, r);

	}

}

void makeBase4(int base10, char *base4, int r){
	int num = base10;

	for(int p = r-2; p >= 0; p--){
		int div = (int)pow(4,p);
		base4[(r-2) -p] = (num/div) + '0';
		num %= div;
	}


}

void showWalk(char *walk, int r){

	for (int i = 0; i < r-1; i++){
		printf("%c", walk[i]);
	}
	printf("\n");

}