/*
This program generates a configuration file for the application to read 
from a simplified format
consisting of * for walls, o for pellets and g for ghosts.
*/

#include <stdio.h>

#define WIDTH 10
#define HEIGHT 25
#define SCALE 2

void writeLine(int id, int x, int y, char *path, const char *filename, FILE *ofp, float scale) {
	fprintf(ofp, "%d\n 1 %d %d 0 0 0 0 %s%s 1 %f\n", id, x*SCALE-(WIDTH*SCALE/2), y*SCALE-(HEIGHT*SCALE/2), path, filename, scale);
	fflush(ofp);
}

int writeLevel(char *filename, char *path) {
	int i=1; int j=0;
	FILE *ifp = fopen(filename, "r");
	FILE *ofp = fopen("tmp.cfg", "w");
	int idx = 0;
	char obj;
	for(obj = fgetc(ifp); !feof(ifp); obj = fgetc(ifp), i++) {
			switch(obj) {
				case '*':
					writeLine(idx, j, i, path, "cube", ofp, 1.0);
					break;
				case 'o':
					writeLine(idx, j, i, path, "pellet", ofp, 0.5);
					break;
				case 'g':
					writeLine(idx, j, i, path, "ghost", ofp, 0.5);
					break;
				case 't':
					writeLine(idx, j, i, path, "tree", ofp, 1.0);
					break;
				case 'p':
					writeLine(idx, j, i, path, "pikachu", ofp, 1.0);
					break;
				case '\n':
					j++;
					i=0;
					break;
			}
			if(obj!='\n') idx++;
		}
}
