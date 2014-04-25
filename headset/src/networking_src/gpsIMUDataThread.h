// File: gpsIMUDataThread.h
// Contains definitions for extracting
// and holding gps and imu data.
#ifndef GPSIMUDATATHREAD_H
#define GPSIMUDATATHREAD_H

#include <stdint.h>

/**
* @brief localHeadsetPos used to keep track of
*		 location info from this headset.
*/
typedef struct localHeadsetPos
{
	uint32_t numSat;  // number of satellites available
  float lat;      		// latitude of headset
  float lon;     		  // longitude of headset
	float x,y;          // coordinates of player
	float roll,pitch,yaw;   // orientation of player
} localHeadsetPos_t;

// Functions
int openComPort();
int getHeadsetPosData(localHeadsetPos_t *pos);
int setGPSOrigin(float lat, float lon);
int getGPSOrigin(float *lat, float *lon);
void *gpsImuThread(void *args);
int initGPSIMUServer(void);
void stopGPSIMUServer(void);

#endif
