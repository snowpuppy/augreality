#ifndef SERVERCOMM_H
#define SERVERCOMM_H

#include <stdint.h>

#define GETBROADCAST 1
#define GETHEARTBEAT 2

/**
* @brief broadCastInfo used to keep track of
*		 broadCast packet info from headsets.
*/
typedef struct broadCastInfo
{
  uint16_t address; // network address of xbee
  float latitude;   // latitude of headset
  float longitude;  // longitude of headset
} broadCastInfo_t;

/**
* @brief heartBeatInfo used to keep track of
*		 information retrieved from a heart beat
*		 packet.
*/
typedef struct heartBeatInfo
{
	uint16_t id;            // id of the player (same as xbee address)
	float x,y;              // coordinates of player
	float roll,pitch,yaw;   // orientation of player
} heartBeatInfo_t;

// This is a supplementarty structure,
// not a packet. This packages the info
// for a headset's position.
typedef struct headsetPos
{
    float x,y;              // coordinates of player
    float roll,pitch,yaw;   // orientation of player
} headsetPos_t;

// FUNCTIONS
void ntohBroadCastInfo(broadCastInfo_t *p);
void htonBroadCastInfo(broadCastInfo_t *p);
void ntohHeartBeatInfo(heartBeatInfo_t *p);
void htonHeartBeatInfo(heartBeatInfo_t *p);

#endif
