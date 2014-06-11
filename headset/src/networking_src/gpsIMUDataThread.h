// File: gpsIMUDataThread.h
// Contains definitions for extracting
// and holding gps and imu data.
#ifndef GPSIMUDATATHREAD_H
#define GPSIMUDATATHREAD_H

#include <stdint.h>

// Approximate battery voltages in millivolts reported by fuel system at empty and full charge
#define BATTERY_EMPTY 2700
#define BATTERY_FULL 3380

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
unsigned int getHeadsetVoltage(void);
int getHeadsetPosData(localHeadsetPos_t *pos);
void getPosFromGPS(float lat, float lon, float *x, float *y);
int setGPSOrigin(float lat, float lon);
int getGPSOrigin(float *lat, float *lon);
void *gpsImuThread(void *args);
int initGPSIMUServer(void);
void stopGPSIMUServer(void);

#endif
