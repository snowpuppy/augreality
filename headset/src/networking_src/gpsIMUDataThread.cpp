#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "gpsIMUDataThread.h"

// Constants
#define BAUDRATE B57600
#define GPSIMUPORT "/dev/ttyACM0"
#define DECIMALSPERDEGLAT 111320
#define DECIMALSPERDEGLON 78710

// Local Function prototypes.
int readBytes(int32_t fd, char *data, int numBytes);
void updatePosition(char *data);

// Global Variables.
// position of this headset
headsetPos_t g_pos;
float originlat = 0;
float originlon = 0;
// port open for gps and imu communication.
int32_t g_port;
pthread_t tidp;
int quit = 0;

// Function: initServer()
// Purpose: starts the server that will
// collect gps and imu data.
int initGPSIMUServer(void)
{
	int ret = 0;
	ret = pthread_create(&tidp, NULL, gpsImuThread, NULL);
	return ret;
}

// Function: stopServer()
// Purpose: stops the server that will
// collect gps and imu data.
void stopGPSIMUServer(void)
{
	// Assert quit signal
	// and join the thread.
	quit = 1;
	pthread_join(tidp, NULL);
}

/**
* @brief gpsImuThread() collects gps and imu
*	data from its associated com port. Start
* with the initServer() function and stop
* with the stopServer() function.
*
* @param args - required argument list.
*
* @return - does not return anything.
*/
void *gpsImuThread(void *args)
{
	char data[64];
	openComPort();
	while (!quit)
	{
		printf("size of float*6: %d\n", sizeof(float)*6);
		readBytes(g_port, data, sizeof(float)*6);
		updatePosition(data);
	}
}


int getHeadsetPosData(headsetPos_t *pos)
{
	// yay for structure copying!
	*pos = g_pos;
}

/**
* @brief update the current position information.
*
* @param data - character array containing info.
*/
void updatePosition(char *data)
{
	int i = 0;
	for (i = 0; i < 24; i++)
	{
		printf("%0.8X",(int)data[i]);
	}
	printf("\n");
	i = 0;
	g_pos.lat = *((float *)&data[i]); i+=sizeof(float);
	g_pos.lon = *((float *)&data[i]); i+=sizeof(float);
	g_pos.pitch = *((float *)&data[i]); i+=sizeof(float);
	g_pos.roll = *((float *)&data[i]); i+=sizeof(float);
	g_pos.yaw = *((float *)&data[i]); i+=sizeof(float);
	g_pos.numSat = (uint32_t) *((float *)&data[i]); i+=sizeof(float);

	// Set x and y based on origin
	g_pos.x = (g_pos.lat - originlat)*DECIMALSPERDEGLAT;
	g_pos.y = (g_pos.lon - originlon)*DECIMALSPERDEGLON;
	// Automatic sanitization of gps coordinates.
	// If no one initializes us, at least we have an "ok" value.
	if (originlat == 0 && originlon == 0 && g_pos.numSat > 2)
	{
		originlat = g_pos.lat;
		originlon = g_pos.lon;
	}
	printf("lat: %0.2f ", g_pos.lat);
	printf("lon: %0.2f ", g_pos.lon);
	printf("pitch: %0.2f ", g_pos.pitch);
	printf("roll: %0.2f ", g_pos.roll);
	printf("yaw: %0.2f ", g_pos.yaw);
	printf("numSat: %d ", g_pos.numSat);
	printf("x: %0.2f ", g_pos.x);
	printf("y: %0.2f \n", g_pos.y);
}

/**
* @brief Set the gps position information.
*
* @param lon - longitude of headset
* @param lat - latitude of headset
*
* @return - success or failure.
*/
int setGPSOrigin(float lon, float lat)
{
	originlat = lat;
	originlon = lon;
	// Always succeeds.
	return 1;
}

/**
* @brief Open com port for gps/imu.
*
* @return file descriptor for open file.
*/
int openComPort()
{
	int res;
	struct termios tio;

	// Open serial port for reading/writing
	g_port = open(GPSIMUPORT, O_RDWR|O_NOCTTY); 
	if (g_port < 0)
	{
		perror(GPSIMUPORT);
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
	printf("bytesRead: %d\n", bytesRead);
	return bytesRead;
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
