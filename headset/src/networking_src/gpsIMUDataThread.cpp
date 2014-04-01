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
#include <time.h>

// Constants
#define BAUDRATE B57600
#define GPSIMUPORT "/dev/ttyACM0"
#define DECIMALSPERDEGLAT 111320
#define DECIMALSPERDEGLON 78710

// if no gps/imu, then fake the data.
#define NOGPSIMU 1

// Local Function prototypes.
int readBytes(int32_t fd, char *data, int numBytes);
void updatePosition(char *data);

// Global Variables.
// position of this headset
localHeadsetPos_t g_pos;
float originlat = 0;
float originlon = 0;
// port open for gps and imu communication.
int32_t g_port;
pthread_t gpsIMUInterfaceTidp;
int gpsIMUInterfaceQuit = 0;

// Function: initServer()
// Purpose: starts the server that will
// collect gps and imu data.
int initGPSIMUServer(void)
{
	int ret = 0;
	ret = pthread_create(&gpsIMUInterfaceTidp, NULL, gpsImuThread, NULL);
	return ret;
}

// Function: stopServer()
// Purpose: stops the server that will
// collect gps and imu data.
void stopGPSIMUServer(void)
{
	// Assert gpsIMUInterfaceQuit signal
	// and join the thread.
	gpsIMUInterfaceQuit = 1;
	pthread_join(gpsIMUInterfaceTidp, NULL);
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
#ifndef NOGPSIMU
	openComPort();
#endif
	while (!gpsIMUInterfaceQuit)
	{
#ifndef NOGPSIMU
		//printf("size of float*6: %d\n", sizeof(float)*6);
		readBytes(g_port, data, sizeof(float)*6);
#endif
		updatePosition(data);
	}
}


int getHeadsetPosData(localHeadsetPos_t *pos)
{
	// yay for structure copying!
	*pos = g_pos;
	return 0;
} 
/**
* @brief update the current position information.
*
* @param data - character array containing info.
*/
void updatePosition(char *data)
{
	int i = 0;
	float lat = 0, lon = 0;
	float roll = 0, pitch = 0, yaw = 0;
	/*
	for (i = 0; i < 24; i++)
	{
		printf("%0.8X",(int)data[i]);
	}
	printf("\n");
	*/
	i = 0;
#ifndef NOGPSIMU
	lat = *((float *)&data[i]); i+=sizeof(float);
	lon = *((float *)&data[i]); i+=sizeof(float);
	pitch = *((float *)&data[i]); i+=sizeof(float);
	roll = *((float *)&data[i]); i+=sizeof(float);
	yaw = *((float *)&data[i]); i+=sizeof(float);
	g_pos.numSat = (uint32_t) *((float *)&data[i]); i+=sizeof(float);
	if (lat < 800.0 & lat > -800.0)
	{
		g_pos.lat = lat;
	}
	if (lon < 800.0 & lon > -800.0)
	{
		g_pos.lon = lon;
	}
	if (roll < 180.0 & roll > -180.0)
	{
		g_pos.roll = roll;
	}
	if (pitch < 180.0 & pitch > -180.0)
	{
		g_pos.pitch = pitch;
	}
	if (yaw < 180.0 & yaw > -180.0)
	{
		g_pos.yaw = yaw;
	}
#else
	// Hardcode values for now. Will make them update
	// on regular intervals later.
	// From: 40.42864, -86.92947
	// To: 40.43019, -86.92995
	if (originlat == 0 && originlon == 0)
	{
		originlat = 40.43019;
		originlon = -86.92995;
	}
	g_pos.lat = 40.42864;
	g_pos.lon = -86.92947;
	g_pos.pitch = 40.0;
	g_pos.roll = 22.0;
	g_pos.yaw = -121.0;
	g_pos.numSat = 2;
#endif

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
	/*
	printf("lat: %0.2f ", g_pos.lat);
	printf("lon: %0.2f ", g_pos.lon);
	printf("pitch: %0.2f ", g_pos.pitch);
	printf("roll: %0.2f ", g_pos.roll);
	printf("yaw: %0.2f ", g_pos.yaw);
	printf("numSat: %d ", g_pos.numSat);
	printf("x: %0.2f ", g_pos.x);
	printf("y: %0.2f \n", g_pos.y);
	*/
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

/*
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
*/
