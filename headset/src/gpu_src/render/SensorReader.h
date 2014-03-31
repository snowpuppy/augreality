/*
 * Filename: SensorReader.h
 * Author: Thor Smith
 * Purpose: Create an interface to retrieve 
 *          sensor information from a central location.
 */
#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include "../../threadInterface.h"
#include "../../gpsIMUDataThread.h"
#include <irrlicht.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

class SensorReader
{
	public:
	// Function: initServer()
	// Purpose: starts the server that will
	// communicate with the gui and sensor data.
	int initServer(void);
	// Function: stopServer()
	// Purpose: stops the server that will
	// communicate with the gui and sensor data.
	void stopServer(void);
	// Function: getLocation()
	// Purpose: returns a vector of the location
	// of the current headset.
	irr::core::vector3df getLocation();
	// Function: getOrientation()
	// Purpose: returns a vector of the orientation 
	// of the current heaset.
	irr::core::vector3df getOrientation();
	uint8_t getWifiStatus(void);
	uint8_t getBatteryStatus(void);
	uint32_t getNumSatellites(void);
	private:
	void setUserPos(void);
	void setWifiStatus(void);
	void setBatteryStatus(void);
	static void *_threadServer(void *This);
	void threadServer();
	int connectToServer(int port);
	private:
	localHeadsetPos_t pos;
	uint8_t wifiStatus;
	uint8_t batteryStatus;
	pthread_t tidp;
	pthread_mutex_t mutex;
	int quit;
};

#endif
