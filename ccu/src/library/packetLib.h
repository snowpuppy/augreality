// File: packetLib.h
// Contains the definition of the
// packets exchanged wirelessly. The
// header and crc data will be added later.
#ifndef PACKET_H
#define PACKET_H
#include "packets.h"

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

// Functions
int openComPort();
void addHeader(uint8_t *buf);
uint8_t loadStaticDataToBytes(loadStaticData_t *p, uint8_t *buf);
int16_t findHeartBeating(uint32_t id);
int16_t findBroadCasting(uint16_t id);
int16_t goBack(uint16_t id);
int16_t getPos(headsetPos_t *pos, uint16_t id);
int16_t getPos(headsetPos_t *pos, uint16_t id);
uint16_t getNumAlive();
uint16_t getAlive(uint16_t id);
int16_t updateObjs(objInfo_t *objList, uint8_t numObjects);
int16_t sendFile(char *filename);
int16_t endSimulationID(uint16_t destId);
int16_t startSimulation();
int16_t acceptID(uint16_t ccuId, uint16_t destId, float originLat, float originLon );
int16_t getBroadCastingIDs(uint16_t *ids, uint16_t size);
uint16_t getNumBroadCasting();
int16_t getBroadCastingLoc(headsetPos_t *pos, uint16_t id);
uint16_t calcCrc(char *packet, int size);

#endif
