#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define GETXBEEDATA 3
#define CLEARSPIBUFFER 2
#define CHIP_SELECT 1
#define SPI_CLK 4000000
#define FILENAME "output.txt"

int main(void) {
	unsigned char buffer[256] = {0};
	unsigned char data = CLEARSPIBUFFER;
  int fd = 0;
	uint32_t filesize = 0;
  FILE *filefp = NULL;

	//SPI setup
	wiringPiSetup();
	fd = wiringPiSPISetup(0, SPI_CLK);
  if (fd < 0)
  {
    fprintf(stderr, "Error opening SPI!\n");
    exit(1);
  }

  filefp = fopen(FILENAME, "wb");
  if (filefp == NULL)
  {
    fprintf(stderr, "Could not open file: %s\n", FILENAME);
    exit(1);
  }

	// GPIO for chip select
	pinMode(CHIP_SELECT, OUTPUT);

	// clear spi buffer
	wiringPiSPIDataRW(0, &data, 1);
	
	// Wait for data from XBEE
	while (data == 0)
	{
		// send command
		data = GETXBEEDATA;
		wiringPiSPIDataRW(0, &data, 1);
		// sleep for 10 microseconds
		// to give processor time to copy data.
		usleep(10);
		data = 0;
		wiringPiSPIDataRW(0, &data, 1);
	}
	usleep(10);
	// Read in type of data and then read
	// the file size
	wiringPiSPIDataRW(0,buffer,(uint32_t)data);
	filesize = *((uint32_t *)&buffer[1]);
	printf("Read %d bytes for packet of type %d, filesize = %d\n", (uint32_t)data, (uint32_t)buffer[0], filesize);

	// Write first set of bytes to the file.
	fwrite(&buffer[5], 1, (uint32_t)(data-5), filefp);

	// decrement remaining bytes
	// to be read.
	filesize -= ((uint32_t)data-5);

	// Get entire file and write
	// to output file.
  while (filesize > 0)
	{
		// Wait for data from XBEE
		while (data == 0)
		{
			// send command
			data = GETXBEEDATA;
			wiringPiSPIDataRW(0, &data, 1);
			// sleep for 10 microseconds
			// to give processor time to copy data.
			usleep(10);
			data = 0;
			wiringPiSPIDataRW(0, &data, 1);
		}
		usleep(10);

		wiringPiSPIDataRW(0,buffer,(uint32_t)data);
		fwrite(buffer, 1, (uint32_t)data, filefp);
		filesize -= (uint32_t)data;
	}
  fclose(filefp);

	return 0;
}
