/*
This program generates a configuration file for the application to read 
from a simplified format
consisting of * for walls, o for pellets and g for ghosts.
*/

#include <stdio.h>

#define WIDTH 10
#define HEIGHT 25
#define SCALE 2

void writeLine(int id, int x, int y, char *filename, FILE *ofp, float scale) {
	fprintf(ofp, "%d\n 1 %d %d 0 0 90 0 %s 1 %f\n", id, x*SCALE-(WIDTH*SCALE/2), y*SCALE-(HEIGHT*SCALE/2), filename, scale);
}

int main(void) {
	FILE *ifp = fopen("maze", "r");
	FILE *ofp = fopen("config.txt", "w");
	int idx = 0;
	char obj;
	for(int i=0; i<HEIGHT; i++) {
		for(int j=0; j<WIDTH; j++) {
			obj = fgetc(ifp);
			switch(obj) {
				case '*':
					writeLine(idx, j, i, "wall", ofp, 1.0);
					break;
				case 'o':
					writeLine(idx, j, i, "pellet", ofp, 0.5);
					break;
				case 'g':
					writeLine(idx, j, i, "ghost", ofp, 0.5);
					break;
			}
			if(obj!='\n') idx++;
		}
		fgetc(ifp);
	}

	
}
