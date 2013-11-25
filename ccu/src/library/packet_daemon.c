#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "packets.h"

// Constants
#define BAUDRATE B57600
#define XBEEPORT "/dev/ttyUSB0"

// Other functions.
int readBytes(int fd, char *data, int numBytes);

// Main Function.
int main(void)
{
	int fd = 0;
	char buf[255];
	char crc = 0;

	char temp;
	broadCastPacket_t packet;
	fd = openComPort();
	//res = write(fd,"abcdh",5);
	while (1)
	{
		readBytes(fd,buf,3);
		if (buf[0] == 'P' && buf[1] == 'A' && buf[2] == 'C')
		{
			printf("Recieving packet...\n");
		}
		else
		{
			//printf("Received dead stuff...\n");
			continue;
		}
		readBytes(fd,buf,1);
		if (buf[0] == (char)0)
		{
			printf("Received a broadcastPacket....\n");
		}
		else
		{
			printf("Received a packet of type %d....\n",(int)(buf[0]));
			continue;
		}
		readBytes(fd,buf,2);
		packet.address = *((short *)buf);
		printf("Wireless ID: %X\n", packet.address);
		readBytes(fd,buf,4);
		//printf("lattitude: [0] = %X, [1] = %X, [2] = %X, [3] = %X\n", buf[0], buf[1], buf[2], buf[3]);
		packet.latitude = *((float *)buf);
		//printf("Lattitude: %X\n", packet.latitude);
		//printf("Lattitude: %d\n", packet.latitude);
		printf("Lattitude: sizeoffloat = %d, %f\n", sizeof(float), packet.latitude);//(float)packet.latitude/10000.0);
		readBytes(fd,buf,4);
		//printf("longitude: [0] = %X, [1] = %X, [2] = %X, [3] = %X\n", buf[0], buf[1], buf[2], buf[3]);
		packet.longitude = *((float *)buf);
		//printf("Longitude: %X\n", packet.longitude);
		//printf("Longitude: %d\n", packet.longitude);
		printf("Longitude: %f\n", packet.longitude);//(float)packet.longitude/10000.0);
		readBytes(fd,buf,2);
		crc = *((short *)buf);
		printf("Crc: %X\n", crc);
	}
	return 0;
}

int openComPort();
{
	int fd, res;
	struct termios tio;

	// Open serial port for reading/writing
	fd = open(XBEEPORT, O_RDWR|O_NOCTTY); 
	if (fd < 0)
	{
		perror(XBEEPORT);
		exit(1);
	}

	// Clear and then configure serial port
	bzero(&tio, sizeof(tio));
	tio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	// Set input mode (non-canonical, no echo,...)
	tio.c_lflag = 0;
	tio.c_cc[VTIME]    = 0;
	// Set number of characters to block until received
	tio.c_cc[VMIN]     = 1;

	// Flush the serial port
	tcflush(fd, TCIFLUSH);
	// Configure the serial port
	tcsetattr(fd,TCSANOW,&tio);

	return fd;
}

// Function to read in a fixed number
// of bytes from the serial stream.
// User is responsible for pointer size.
int readBytes(int fd, char *data, int numBytes)
{
  int bytesRead = 0;
  int res = 0;
  //printf("Reading %d bytes.\n", numBytes);
  while (bytesRead < numBytes)
  {
    res = read(fd, data, numBytes - bytesRead);
    //printf("%d bytes read.\n", res);
    if (res < 0)
    {
      perror("Error reading serial data.\n");
    }
    else
    {
      // increment number of bytes read.
      // Keep reading till the expected number
      // of bytes is read.
      bytesRead += res;
    }
  }
}

// Function: printFloatBytes
// Purpose: Used to print the hex
// values of a floating point number
// as a sanity check that they are
// coming in correctly.
void printFloatBytes(char *buf)
{
  int i = 0;
  printf("Floating value:");
  for (i = 0; i < 4; i++)
  {
    printf(" [%d] = %X",i,buf[i]);
  }
  printf("\n");
}
