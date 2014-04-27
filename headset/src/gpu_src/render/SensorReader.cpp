/*
 * Filename: SensorReader.cpp
 * Author: Thor Smith
 * Purpose: Create an interface to retrieve 
 *          sensor information from a central location.
 */
#include "SensorReader.h"

// CONSTANTS
#define PORT 7777

// Function: initServer()
// Purpose: starts the server that will
// communicate with the gui and simulation.
int SensorReader::initServer(void)
{
	int ret = 0;
	tidp = 0;
	quit = 0;
	// Clear the pos structure
	memset(&pos,0,sizeof(pos));
	// create mutex w/ default attrs
	ret = pthread_mutex_init(&mutex, NULL);
	if (ret < 0)
	{
		perror("Error: Could not create mutex!\n");
		return ret;
	}
	ret = pthread_create(&tidp, NULL, SensorReader::_threadServer, this);
	return ret;
}

// Function: stopServer()
// Purpose: stops the server that will
// communicate with the gui and simulation.
void SensorReader::stopServer(void)
{
	// Assert quit signal
	// and join the thread.
	quit = 1;
	// destroy mutex w/ default attrs
	pthread_mutex_destroy(&mutex);
	pthread_join(tidp, NULL);
}

/**
* @brief Dummy function to start actual thread.
*
* @param This - pointer to class running thread.
*
* @return nothing.
*/
void *SensorReader::_threadServer(void *This)
{
	((SensorReader *)This)->threadServer();
}

// Function: threadServer()
// Purpose: starting function for the
// thread.
void SensorReader::threadServer()
{
	while (!quit)
	{
		// Establish TCP port connection.
		setUserPos();
		setWifiStatus();
		setBatteryStatus();
		// sleep for 1 millisecond
		usleep(1000);
	}
}

void SensorReader::setUserPos(void)
{
	int32_t fd = 0, rc = 0;
	char command = GETUSERPOS;
	localHeadsetPos_t _pos;
	// Open connection.
  fd = connectToServer(PORT);
  if (fd < 0)
  {
    perror("setUserPos:Error setting up server.\n"); exit(1);
  }
	rc = write(fd, (void *)&command, sizeof(command));
	if (rc < 0)
	{
    perror("setUserPos:Error writing to socket.\n"); exit(1);
	}
	rc = read(fd, (void *)&_pos, sizeof(_pos));
	if (rc < 0)
	{
    perror("setUserPos:Error reading from socket.\n"); exit(1);
	}
	// copy the structure
	pthread_mutex_lock(&mutex);
	pos = _pos;
	pthread_mutex_unlock(&mutex);
	// Close socket.
  close(fd);
}

void SensorReader::setWifiStatus(void)
{
	int32_t fd = 0, rc = 0;
	char command = GETWIFISTATUS;
	uint8_t _wifi = 0;
	// Open connection.
  fd = connectToServer(PORT);
  if (fd < 0)
  {
    perror("setWifiStatus:Error setting up server.\n"); exit(1);
  }
	rc = write(fd, (void *)&command, sizeof(command));
	if (rc < 0)
	{
    perror("setWifiStatus:Error: writing to socket.\n"); exit(1);
	}
	rc = read(fd, (void *)&_wifi, sizeof(_wifi));
	if (rc < 0)
	{
    perror("setWifiStatus:Error reading from socket.\n"); exit(1);
	}
	// copy the structure
	pthread_mutex_lock(&mutex);
	wifiStatus = _wifi;
	pthread_mutex_unlock(&mutex);
	// Close socket.
  close(fd);
}

void SensorReader::setBatteryStatus(void)
{
	int32_t fd = 0, rc = 0;
	char command = GETBATTERYSTATUS;
	uint8_t _battery = 0;
	// Open connection.
  fd = connectToServer(PORT);
  if (fd < 0)
  {
    perror("setBatteryStatus:Error setting up server.\n"); exit(1);
  }
	rc = write(fd, (void *)&command, sizeof(command));
	if (rc < 0)
	{
    perror("setBatteryStatus:Error: writing to socket.\n"); exit(1);
	}
	rc = read(fd, (void *)&_battery, sizeof(_battery));
	if (rc < 0)
	{
    perror("setBatteryStatus:Error reading from socket.\n"); exit(1);
	}
	// copy the structure
	pthread_mutex_lock(&mutex);
	batteryStatus = _battery;
	pthread_mutex_unlock(&mutex);
	// Close socket.
  close(fd);
}

irr::core::vector3df SensorReader::getLocation()
{
	float x, y;
	pthread_mutex_lock(&mutex);
	x = pos.x;
	y = pos.y;
	pthread_mutex_unlock(&mutex);
	return irr::core::vector3df(-y, x, 0.0f);
}

irr::core::vector3df SensorReader::getOrientation()
{
	float roll,pitch,yaw;
	pthread_mutex_lock(&mutex);
	roll = pos.roll;
	pitch = pos.pitch;
	yaw = pos.yaw;
	pthread_mutex_unlock(&mutex);
	return irr::core::vector3df(roll, pitch, yaw);
}

headsetLocation_t SensorReader::_getLocation()
{
	headsetLocation_t loc;
	pthread_mutex_lock(&mutex);
	loc.x = pos.x;
	loc.y = pos.y;
	pthread_mutex_unlock(&mutex);
	return loc;
}

headsetOrientation_t SensorReader::_getOrientation()
{
	headsetOrientation_t orient;
	pthread_mutex_lock(&mutex);
	orient.roll = pos.roll;
	orient.pitch = pos.pitch;
	orient.yaw = pos.yaw;
	pthread_mutex_unlock(&mutex);
	return orient;
}

uint8_t SensorReader::getWifiStatus(void)
{
	uint8_t _wifi = 0;
	pthread_mutex_lock(&mutex);
	_wifi = wifiStatus;
	pthread_mutex_unlock(&mutex);
	return _wifi;
}

uint8_t SensorReader::getBatteryStatus(void)
{
	uint8_t _battery = 0;
	pthread_mutex_lock(&mutex);
	_battery = batteryStatus;
	pthread_mutex_unlock(&mutex);
	return _battery;
}

uint32_t SensorReader::getNumSatellites(void)
{
	uint32_t sats = 0;
	pthread_mutex_lock(&mutex);
	sats = pos.numSat;
	pthread_mutex_unlock(&mutex);
	return sats;
}

// Connects to INADDR_LOOPBACK
int SensorReader::connectToServer(int port)
{
  int fd = 0;
	int rc = 0;
  struct sockaddr_in sockAddr = {0};

  // Fill out connection structure.
  sockAddr.sin_port = htons(port);
  sockAddr.sin_family = AF_INET;
  
  sockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  // Create a socket.
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if ( fd < 0 )
  {
		printf("Error creating socket!\n");
    return -1;
  }

  //Connect to server
  rc = connect(fd, (const struct sockaddr *) &sockAddr, sizeof(sockAddr) );
  if ( rc < 0 )
  {
		printf("Error connecting to port %d!\n", port);
    return -1;
  }
	return fd;
}
