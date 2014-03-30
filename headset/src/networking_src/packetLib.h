// File: packetLib.h
// Contains the definition of the
// packets exchanged wirelessly. The
// header and crc data will be added later.
#ifndef PACKETLIB_H
#define PACKETLIB_H
#include "packets.h"

// Constants
#define MAXSIZEOFID 16

// Api requests made from gui
// and from the simulation to
// get updated information
// about the headsets.
enum apiRequest
{
	GETBROADCASTIDS = 1,
	GETNUMBROADCAST,
	GETBROADCASTLOC,
	GETPOSITION,
	GETNUMALIVE,
	GETALIVE,
	SENDUPDATEOBJS,
	SENDFILE,
	SENDEND,
	SENDSTART,
	SENDACCEPT,
	SENDGOBACK
};

/**
* @brief broadCastInfo used to keep track of
*		 broadCast packet info from headsets.
*		 all it needs is the id of the headset
*		 and the ip address of the headset.
*/
typedef struct broadCastInfo
{
  uint8_t name[MAXSIZEOFID]; // Name of headset (default to HEAD<mac>)
	uint32_t ip_addr; // Ip address of headset
} broadCastInfo_t;

/**
* @brief heartBeatInfo used to keep track of
*		 information retrieved from a heart beat
*		 packet.
*/
typedef struct heartBeatInfo
{
	uint8_t name[MAXSIZEOFID];  			// name of headset
	float x,y;              // coordinates of player
	float roll,pitch,yaw;   // orientation of player
} heartBeatInfo_t;

// This is a supplementarty structure,
// not a packet. This packages the info
// for a headset's position.
typedef struct headsetPos
{
		float lat,lon;
    float x,y;              // coordinates of player
    float roll,pitch,yaw;   // orientation of player
} headsetPos_t;

// Functions
int16_t findHeartBeating(uint8_t *id);
int16_t findBroadCasting(uint8_t *id);
int16_t goBack(uint8_t *id);
int16_t getPos(headsetPos_t *pos, uint8_t *id);
uint16_t getNumAlive();
uint16_t getAlive(uint8_t *id);
uint16_t addAliveID(uint8_t *id);
int16_t updateObjs(objInfo_t *objList, uint8_t numObjects);
int16_t sendFile(char *filename);
int16_t endSimulationID(uint8_t *destId);
int16_t startSimulation();
int16_t acceptID(uint8_t *ccuId, uint8_t *destId, float originLat, float originLon );
int16_t getBroadCastingIDs(uint8_t *ids, uint16_t numIds);
uint16_t getNumBroadCasting();
int16_t getBroadCastingLoc(headsetPos_t *pos, uint8_t *id);
int16_t writeByteStream(uint8_t *buf, uint16_t size);
uint8_t detectHeader(uint8_t *pac);
void getHeartBeatPacket(void);
void getBroadCastPacket(void);

#endif
