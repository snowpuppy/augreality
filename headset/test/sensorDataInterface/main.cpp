#include <stdio.h>
#include "SensorReader.h"

int main(void)
{
	headsetLocation_t loc = {0};
	headsetOrientation_t ori = {0};
	SensorReader reader;
	reader.initServer();
	while (1)
	{
		loc = reader._getLocation();
		ori = reader._getOrientation();
		printf("\rLoc: %0.2f, %0.2f Ori: %0.2f, %0.2f, %02.f", loc.x,loc.y,ori.pitch,ori.roll,ori.yaw);
		usleep(10000);
	}
}
