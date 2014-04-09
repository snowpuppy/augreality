// File: packetLib.h
// Contains the definition of the
// packets exchanged wirelessly. The
// header and crc data will be added later.
#ifndef PACKETLIB_H
#define PACKETLIB_H
#include "packets.h"

// Constants
#define DEFAULT_TCP_PORT 7788
#define DEFAULT_UDP_PORT 7789
#define HEADSETNAMESIZE 32

enum headsetState
{
	INIT = 0, 		// Transition to BROADCAST on setting client or HOSTSIMULATION on server start.
	BROADCAST, 		// Transition to ACCEPTED on receipt of Accept packet
	ACCEPTED, 		// Transition to SIMULATION on receipt of Start Packet
	SIMULATION, 	// Transition to INIT on receipt of End Packet
	HOSTSIMULTION
};

/**
* @brief broadCastInfo used to keep track of
*		 broadCast packet info from headsets.
*		 all it needs is the id of the headset
*		 and the ip address of the headset.
*/
typedef struct broadCastInfo
{
  uint8_t name[HEADSETNAMESIZE]; // Name of headset (default to HEAD<mac>)
	uint32_t ip_addr; // Ip address of headset
} broadCastInfo_t;

/**
* @brief heartBeatInfo used to keep track of
*		 information retrieved from a heart beat
*		 packet.
*/
typedef struct heartBeatInfo
{
	uint8_t name[HEADSETNAMESIZE];  			// name of headset
	uint32_t ip_addr; 			// Ip address of headset
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
void getHeartBeatPacket(void);
int checkForNewPackets(int fd1, int fd2);
int wirelessConnection();
int bindToTcpServer(uint16_t port);
int bindToUdpPort(int port);
int connectToServer(int port, uint32_t addr);
void sendUpdatePacket(int udpFd, int *state);
void sendBroadcast(int udpFd);
void processPacket(int udpFd, int tcpFd, int ret, int pType, int *state);
int32_t writeTcpByteStream(void *buf, uint32_t size);
int32_t writeUdpByteStream(void *msg, uint32_t size, uint32_t dest);
int32_t readTcpByteStream(void *buf, uint32_t size);
int32_t readUdpByteStream(void *buf, uint32_t size, uint32_t peek = 0);
uint8_t detectUdpType(int fd);
uint8_t detectTcpType(int fd);
void getBroadCastPacket(void);

#endif
