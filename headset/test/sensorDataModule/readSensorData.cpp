#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

// Constants
#define BAUDRATE B115200
#define XBEEPORT "/dev/ttyACM0"

// Global variables.
int32_t g_port;

int readBytes(int fd, char *data, int numBytes);
int openComPort();

int main(void)
{
	float roll = 0,pitch = 0,yaw = 0,x = 0,y = 0;
	char data[20];
	int fd = 0, i = 0;
	fd = openComPort();
	while(1)
	{
		i = 0;
		readBytes(fd, data, sizeof(float)*5);
		//printf("sizeoffloat*4 = %d\n", sizeof(float)*5);
		//printf("i = %d\n", i);
		x = *((float *)&data[i]); i+=4;
		//printf("i = %d\n", i);
		y = *((float *)&data[i]); i+=4;
		//printf("i = %d\n", i);
		pitch = *((float *)&data[i]); i+=4;
		//printf("i = %d\n", i);
		roll = *((float *)&data[i]); i+=4;
		//printf("i = %d\n", i);
		yaw = *((float *)&data[i]); i+=4;
		//printf("roll: %0.2f, pitch: %0.2f, yaw: %0.2f, x: %0.2f, y: %0.2f\n", roll, pitch, yaw, x, y);
	}
}

int openComPort()
{
	int res;
	struct termios tio;

	// Open serial port for reading/writing
	g_port = open(XBEEPORT, O_RDWR|O_NOCTTY); 
	if (g_port < 0)
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
	tcflush(g_port, TCIFLUSH);
	// Configure the serial port
	tcsetattr(g_port,TCSANOW,&tio);

	return g_port;
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
