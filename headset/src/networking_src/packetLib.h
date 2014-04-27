// File: packetLib.h
// Contains the definition of the
// packets exchanged wirelessly. The
// header and crc data will be added later.
#ifndef PACKETLIB_H
#define PACKETLIB_H
#include "packets.h"
#include <time.h>
#include <vector>

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
	HOSTSIMULTION // the host of a simulation doesn't need to send a heartbeat.
};

/**
* @brief heartBeatInfo used to keep track of
*		 information retrieved from a heart beat
*		 packet.
*/
typedef struct heartBeatInfo
{
	uint32_t ipAddr; 			// Ip address of headset
	float lat,lon;					// absolute lat/lon coordinates of player.
	float x,y;              // coordinates of player
	float roll,pitch,yaw;   // orientation of player
	time_t lastUpdate;
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
int packetLibInit();
int16_t getPos(headsetPos_t *pos, uint32_t id);
uint16_t getNumAlive();
uint16_t getAlive(uint32_t id);
int16_t getAliveIDs(uint32_t *ids, uint16_t size);
uint16_t addAliveID(uint32_t id);
int16_t updateObjs(objInfo_t *objList, uint32_t numObjects);
int16_t sendFile(char *filename, uint32_t id);
int16_t startSimulation();
int16_t getStartSimulation(int32_t connfd);
int16_t sendDropId(uint32_t destId);
int16_t getDropId(int32_t connfd);
int16_t acceptID(uint32_t destId);
int16_t getAcceptID(int32_t connfd);
int16_t endSimulationID(uint32_t destId);
int16_t getEndSimulation(int32_t connfd);
int16_t getBroadCastingIDs(uint32_t *ids, uint16_t numIds);
uint16_t getNumBroadCasting();
void getHeartBeatPacket(void);
int checkForNewPackets(int fd1, int fd2);
int wirelessConnection();
int bindToTcpServer(uint16_t port);
int bindToUdpPort(int port);
int connectToServer(int port, uint32_t addr);
void sendUpdatePacket(int udpFd);
void sendBroadcast(int udpFd);
void sendHeartbeat(int udpFd);
void processPacket(int udpFd, int tcpFd, int connFd, uint32_t addr, int ret, int pType);
int32_t writeTcpByteStream(void *buf, uint32_t size);
int32_t writeUdpByteStream(void *msg, uint32_t size, uint32_t dest);
int32_t readTcpByteStream(void *buf, uint32_t size);
int32_t readUdpByteStream(void *buf, size_t size, uint32_t *addr, uint32_t peek = 0);
uint8_t detectUdpType(int fd);
uint8_t detectTcpType(int fd, int *connfd, uint32_t *clientAddr);
void getBroadCastPacket(void);
uint32_t getMyId();
uint32_t getState();
uint32_t setHostHeadset(int32_t host);
uint32_t getFileReceived();
void getReceivedFile(char *filename, int size);
int16_t getUpdateObjs(std::vector<objInfo_t> &objs);

#endif
