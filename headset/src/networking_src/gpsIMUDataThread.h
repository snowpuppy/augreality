// File: gpsIMUDataThread.h
// Contains definitions for extracting
// and holding gps and imu data.
#ifndef GPSIMUDATATHREAD_H
#define GPSIMUDATATHREAD_H

/**
* @brief broadCastInfo used to keep track of
*		 broadCast packet info from headsets.
*/
typedef struct headsetPos
{
	uint32_t numSat;  // number of satellites available
  float lat;      		// latitude of headset
  float lon;     		  // longitude of headset
	float x,y;          // coordinates of player
	float roll,pitch,yaw;   // orientation of player
} headsetPos_t;

// Functions
int openComPort();
int getHeadsetPosData(headsetPos_t *pos);
int setGPSOrigin(float lon, float lat);
void *gpsImuThread(void *args);
int initGPSIMUServer(void);
void stopGPSIMUServer(void);

#endif
