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
#define SPI_CLK 1000000
#define FILENAME "output.txt"

int main(void) {
	unsigned char buffer[256] = {0};
	unsigned char data = CLEARSPIBUFFER;
  int fd = 0;
	int32_t filesize = 0;
  FILE *filefp = NULL, *filefp2 = NULL;
  int i = 0;

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
  filefp2 = fopen("out.txt", "w");

	// GPIO for chip select
	pinMode(CHIP_SELECT, OUTPUT);

	// clear spi buffer
	wiringPiSPIDataRW(0, &data, 1);

	// Wait for data from XBEE
	while (data == 0 || data == 1)
	{
		// send command
		data = GETXBEEDATA;
		wiringPiSPIDataRW(0, &data, 1);
		if (data != 0 && data != 1) break;
		// sleep for 10 microseconds
		// to give processor time to copy data.
		data = 0;
		wiringPiSPIDataRW(0, &data, 1);
	}
	// Read in type of data and then read
	// the file size
	memset(buffer, 0, sizeof(buffer));
	wiringPiSPIDataRW(0,buffer,(uint32_t)data);
	filesize = *((uint32_t *)&buffer[1]);
	printf("Bytes: %2X:%2X:%2X:%2X:%2X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
	printf("Read %d bytes for packet of type %d, filesize = %d\n", (int)data, (uint32_t)buffer[0], filesize);

	// Write first set of bytes to the file.
	fwrite(&buffer[5], 1, (uint32_t)(data-5), filefp);
	for (i = 5; i < data; i++)
	{
		fprintf(filefp2, "%02X",buffer[i]);
	}
	fprintf(filefp2, "\n");

	// decrement remaining bytes
	// to be read.
	filesize -= ((uint32_t)data-5);

	// Get entire file and write
	// to output file.
  while (filesize > 0)
	{
		//data = 0;
		// send command
		data = GETXBEEDATA;
		wiringPiSPIDataRW(0, &data, 1);
		data = 0;
		// Wait for data from XBEE
		while (data == 0)
		{
			// sleep for 10 microseconds
			// to give processor time to copy data.
			//if (data == 0)
			//{
				data = 0;
				wiringPiSPIDataRW(0, &data, 1);
			//}
		}
		memset(buffer, 0, sizeof(buffer));
		wiringPiSPIDataRW(0,buffer,(uint32_t)data);
		// Check for confirmation of
		// no data.
		if (data == 1 && buffer[0] == 0)
		{
			continue;
		}

		fwrite(buffer, 1, (uint32_t)data, filefp);
		fflush(filefp);
		for (i = 0; i < data; i++)
		{
			fprintf(filefp2, "%02X",buffer[i]);
		}
		printf("Bytes1: %2X:%2X:%2X:%2X:%2X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
		//printf("Bytes2: %2X:%2X:%2X:%2X\n", buffer[data-4], buffer[data-3], buffer[data-2], buffer[data-1]);
		printf("Read %d bytes, %d left\n", (int)data, filesize);
		fprintf(filefp2, "\n");
		fflush(filefp2);
		filesize -= (uint32_t)data;
	}
  fclose(filefp);

	return 0;
}
