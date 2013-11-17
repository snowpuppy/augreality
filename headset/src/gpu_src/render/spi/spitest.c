#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define CHIP_SELECT 1
#define SPI_CLK 4000000
#define FILENAME "output.tar"

int main(void) {
	const char text[] = "D";
	char buffer[256] = {0};
  int fd = 0;
	unsigned int i = 0;
  unsigned char dataSize = 0;
  unsigned int numBytesToRead = 0;
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

	strcpy(buffer, text);

  while (dataSize == 0)
  {
    //test SPI
    digitalWrite(CHIP_SELECT, HIGH);
    wiringPiSPIDataRW(0, (unsigned char *)buffer, strlen(text));
    digitalWrite(CHIP_SELECT, LOW);
    // Extract the size as a number.
    // The micro should send us one byte
    // to indicate how much data should be read.
    dataSize = *((unsigned char *)buffer);
    if (dataSize != 0)
    {
      fprintf(stdout, "dataSize = %x\n", dataSize);
    }
    // copy original content back to buffer.
    strcpy(buffer, text);
    usleep(1000); // sleep for 1 millisecond
  }
  printf("Done with loop. Receiving data.\n");
  usleep(1000);

  // Read in data from spi and save it to file!
  for (i = 0; i < dataSize; i += numBytesToRead)
  {
    numBytesToRead = ( (255 < dataSize - i) ? 255 : dataSize - i);
    digitalWrite(CHIP_SELECT, HIGH);
    wiringPiSPIDataRW(0, (unsigned char *)buffer, numBytesToRead);
    digitalWrite(CHIP_SELECT, LOW);
    //bytesRead = read(fd,buffer, (255 < bytesRead - i) ? 255 : bytesRead - i);
    buffer[numBytesToRead] = '\0';
    //fprintf(stdout,"i = %d, dataSize = %x, numBytesToRead = %x buffer = %s\n",i,dataSize, numBytesToRead,buffer);
    fprintf(stdout,"%s\n",buffer);
    fwrite(buffer, 1, numBytesToRead, filefp);
  }
  fclose(filefp);

  /*
	// dump the output
	for (i = 0; i < strlen(text); i++)
		printf("%02X(%c) ", (unsigned int)(unsigned char)buffer[i], buffer[i]);
	puts("");
  */

	return 0;
}
