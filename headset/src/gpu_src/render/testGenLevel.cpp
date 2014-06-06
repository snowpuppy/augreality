#include "genLevel.h"
#include <iostream>
#include <stdio.h>

int main(void)
{
	FILE *fp = fopen("../../simulation_src/simulations/pacman/pacman","r");
	char buf[80];
	char buf2[80];
	int num = 0;
	/*
	fscanf(fp, "[%79[a-zA-Z]]=%d", buf, &num);
	printf("Scanned %s %d\n", buf, num);
	while (fscanf(fp, "%79s%*[ ]%79s", buf, buf2) == 2)
	{
		printf("Scanned %s %s\n", buf, buf2);
	}
	fscanf(fp, "%79s", buf);
	printf("Scanned %s\n", buf);
	*/
	writeConfigFile("../../simulation_src/simulations/pacman/pacman", "../../simulation_src/simulations/pacman/models/");
}
