#include <stdio.h>
#include "gpuPyThreadInterface.h"

int main(void)
{
	GpuPyThreadInterface inter;
	bool quit = false;
	inter.initServer(&quit);
	while(1)
	{
	}
}
