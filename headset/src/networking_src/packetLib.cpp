#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include "packets.h"
#include "packetLib.h"
#include "threadInterface.h"
#include "gpsIMUDataThread.h"

// Constants
#define BAUDRATE B57600
#define XBEEPORT "/dev/ttyUSB0"
#define LISTENQ 100
// update every 50 milliseconds (in useconds)
#define UPDATE_INTERVAL 50000

// Local Function prototypes.
//int readBytes(int fd, char *data, int numBytes);

// Global Variables.
std::map<std::string, broadCastInfo_t> broadCastList;
std::map<std::string, heartBeatInfo_t> heartBeatList;
// 
int32_t g_packetLibPort;
int32_t g_udpPort = DEFAULT_UDP_PORT;
int32_t g_tcpPort = DEFAULT_TCP_PORT;
int32_t g_myIp = 0;
int32_t g_myMask = 0;
int32_t g_myBroadCast = 0;
int32_t g_udpFd = 0;
int32_t g_tcpFd = 0;

// API FUNCTIONS

// Function: getBroadCastingLoc(id)
// Returns an object for a given id that specifies
// the location information for that object.
int16_t getBroadCastingLoc(headsetPos_t *pos, uint8_t *id)
{
	/*
	std::map<std::string, broadCastInfo_t>::iterator mapIt;
	// Assume incoming string id is NULL terminated!
	mapIt = broadCastList.find(std::string(id,MAXNUMHEADSETS));
	if (mapIt != broadCastList.end())
	{
		*pos = mapIt->second;
	}
	*/
  return 0;
}

// Function: getNumBroadCasting()
// Return the number of Headsets
// currently broadcasting.
uint16_t getNumBroadCasting()
{
	/*
	return g_numBroadCasting;
	*/
	return 0;
}

// Function: getBroadCastingIDs()
// Copies id numbers into the list and returns
// the number of values copied.
// Returns the number of IDs in the array, -1 on error.
int16_t getBroadCastingIDs(uint8_t *ids, uint16_t numIds)
{
	/*
	int16_t i = 0;
	for (i = 0; i < numIds && i < g_numBroadCasting; i++)
	{
		strncpy(&ids[i*SIZEOFID], g_broadCasting[i].address, SIZEOFID);
	}
	return i;
	*/
	return 0;
}

// Function: acceptID(id)
int16_t acceptID(uint8_t *ccuId, uint8_t *destId, float originLat, float originLon )
{
	/*
	uint16_t i = 0;
	// Create a packet and stuff it.
	acceptHeadset_t p = {0};
	p.packetType = BROADCASTPACKET;
	for (i = 0; i < SIZEOFID; i++)
	{
		p.id[i] = ccuId[i];
	}
	p.x = originLat;
	p.y = originLon;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
	*/
  return 0;
}
// startSimulation()
int16_t startSimulation()
{
	/*
	startSimulation_t p = {0};
	p.packetType = STARTSIMULATION;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
	*/
  return 0;
}
// endSimulationID(id)
int16_t endSimulationID(uint8_t *destId)
{
	/*
	endSimulation_t p = {0};
	p.packetType = ENDSIMULATION;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
	*/
	return 0;
}
// sendFile(filename)
int16_t sendFile(char *filename)
{
	loadStaticData_t p = {0};
  uint8_t buf[LOADSTATICDATASIZE + HEADERSIZE + CRCSIZE];
  uint8_t fileBuf[256];
  uint16_t bytesRead = 0, bytesSent = 0, totalBytesSent = 0, bytesRemaining = 0;
  FILE *fp = NULL;

  fp = fopen(filename,"rb");
  if (fp == NULL)
  {
    fprintf(stderr,"Could not open file: %s", filename);
    return -1;
  }
  // Find size of file.
  fseek(fp,0,SEEK_END);
  p.numBytes = ftell(fp);
  rewind(fp);
	p.packetType = LOADSTATICDATA;
	// Get size of file.
	//p.numBytes = ;
  printf("Sending file of size: %d\n", p.numBytes);
	// Pack the packet to a byte stream.
  //loadStaticDataPack(&p,&buf[HEADERSIZE]);
	// Add header info and crc.
  //addHeader(buf);
	// Write the packet to the serial port.
  write(g_packetLibPort, buf, LOADSTATICDATASIZE + HEADERSIZE);
  // Write the file to the serial port
  while ( !feof(fp))
  {
    bytesRead = fread(fileBuf,1,256,fp);
		bytesRemaining = bytesRead;
		bytesSent = 0;
		// While bytes remaining to be sent
		while (bytesRemaining > 0)
		{
			// Write remaining bytes to output.
			bytesSent += write(g_packetLibPort, &fileBuf[bytesRead - bytesRemaining], bytesRemaining);
			printf("Sent %d bytes, %d bytes total...\n", bytesSent, totalBytesSent+bytesSent);
			// Decrement bytes remaining by bytes sent.
			bytesRemaining = bytesRead - bytesSent;
		}
		totalBytesSent += bytesSent;
  }
	fclose(fp);
  return 0;
}

// updateObjs(objInfo *objList)
int16_t updateObjs(objInfo_t *objList, uint8_t numObjects)
{
	/*
	// Static update number increments for each
	// packet sent.
	static uint8_t updateNumber = 0;
	updateObjInstance_t p = {0};

  p.packetType = UPDATEOBJINSTANCE;
  p.numObj = numObjects;
  p.updateNumber = updateNumber++;
  p.objList = objList;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
	*/
  return 0;
}
// getAlive(id)
uint16_t getAlive(uint8_t *id)
{
	/*
	int16_t ret = 0;
	ret = findHeartBeating(id);
	return (ret > 0 ? 1 : 0);
	*/
	return 0;
}
// getNumAlive()
uint16_t getNumAlive()
{
	/*
	return g_numHeartBeating;
	*/
	return 0;
}

// AddAliveID
uint16_t addAliveID(uint8_t *id)
{
	/*
	int16_t ret = 0;
	uint16_t i = 0;
	ret = findHeartBeating(id);
	// If not already found, add it.
	if (ret == -1)
	{
		i = g_numHeartBeating;
		strncpy(g_heartBeating[i].id, id, SIZEOFID/2);
		g_numHeartBeating++;
		return 1;
	}
	*/
	return 0;
}

// getAliveIDs()
int16_t getAliveIDs(uint8_t *ids, uint16_t size)
{
	/*
	int16_t i = 0, j = 0;
	for (i = 0; i < g_numHeartBeating && i < size; i++)
	{
		for (j = 0; j < SIZEOFID; j++)
		{
			ids[i*SIZEOFID + j] = g_heartBeating[i].id[j];
		}
	}
	return i;
	*/
	return 0;
}
// Function: getPos(id)
// Sets position information for headset with id.
// Returns -1 on error.
int16_t getPos(headsetPos_t *pos, uint8_t *id)
{
	/*
	int16_t index = 0;
	if (pos == NULL)
	{
		return -1;
	}
	index = findHeartBeating(id);
	if (index >= 0)
	{
		pos->x = g_heartBeating[index].x;
		pos->y = g_heartBeating[index].y;
		pos->roll = g_heartBeating[index].roll;
		pos->pitch = g_heartBeating[index].pitch;
		pos->yaw = g_heartBeating[index].yaw;
	}
	return index;
	*/
	return 0;
}
// Function: goBack(id)
// Send a goBack message to the headset
// indicating that it should transition to
// an earlier state.
int16_t goBack(uint8_t *id)
{
	/*
	goBack_t p = {0};
	p.packetType = GOBACK;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
	*/
  return 0;
}


// NON_API FUNCTIONS

// Function: findBroadCastingLoc
// Purpose: abstract process of finding ids
// 			in the data structure they are in.
// Returns -1 on error
int16_t findBroadCasting(uint8_t *id)
{
	/*
	int16_t i = 0;
	for (i = 0; i < MAXNUMHEADSETS; i++)
	{
		if (strncmp(g_broadCasting[i].address, id, MAXSIZEOFID) == 0)
		{
			return i;
		}
	}
	return -1;
	*/
	return 0;
}

/**
* @brief checkForNewPackets multiplexes
*				 between a tcp and udp connection.
*
* @param fd1 - udp file descriptor
* @param fd2 - tcp file descriptor
*
* @return 0 if timed out, 1 if packet for
*					first file descriptor. 2 if packet
*					for second file descriptor.
*/
int checkForNewPackets(int fd1, int fd2)
{
  static struct timeval tv = {0};
  fd_set rfds;
	int rc = 0;
	int ret = 0;

	// update time if necessary.
	// may want to set a flag
	// to say that stuff should be sent.
	if (tv.tv_sec == 0 && tv.tv_usec == 0)
	{
		tv.tv_sec = 0;
		tv.tv_usec = UPDATE_INTERVAL;
	}
	// Setup select
	FD_ZERO(&rfds);
	FD_SET(fd1, &rfds);
	FD_SET(fd2, &rfds);
	// Needs largest file descriptor plus 1
	rc = select( fd1>fd2 ? fd1+1 : fd2+1, &rfds, NULL, NULL, &tv);

	// check if a packet is available.
	// return the number associated w/
	// the file descriptor
	if (rc > 0)
	{
		ret = FD_ISSET(fd1, &rfds) ? 1 : FD_ISSET(fd2, &rfds) ? 2 : 0;
	}
	// return whether or not a packet is available.
	return ret;
}

/**
* @brief wirelessConnection extracts
*				 the wireless IP address of this
*				 device to be shared on the network.
*				 TODO: Find out how to get rssi.
*
* @return -1 on failure.
*/
int wirelessConnection()
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;

	if (getifaddrs(&ifaddr) == -1)
	{
		perror("Could not get interfaces for this device!");
		return -1;
	}

	// Loop through all interfaces.
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		// Skip interfaces w/ a null addr.
		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET && strcmp(ifa->ifa_name,"wlan0") == 0 )
		{
			g_myIp = ntohl(((struct sockaddr_in *)(ifa->ifa_addr))->sin_addr.s_addr);
			g_myMask = ntohl(((struct sockaddr_in *)(ifa->ifa_netmask))->sin_addr.s_addr);
			// Calculate broadcast address.
			g_myBroadCast = g_myIp | (~g_myMask);
		}
	}
	freeifaddrs(ifaddr);
	return 0;
}

/**
* @brief bindToTcpServer
*
* @param port - TCP port to bind to.
*
* @return - file descriptor for connection.
*						or -1 for error.
*/
int bindToTcpServer(uint16_t port)
{
  int ret = 0;
  // connection variables
  //
  int listenfd = 0, optval = 1;
  struct sockaddr_in serveraddr = {0};

  // Start listening on port
  // And create the listening Socket
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0)
  {
    fprintf(stderr, "Could not connect to port %d!\n", port); return -1;
  }

  ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));
  if (ret < 0)
  {
    perror("Could not set options!\n"); return -1;
  }

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(port);

  ret = bind(listenfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
  if (ret < 0)
  {
    fprintf(stderr, "Unable to bind to port %d!\n", port); return -1;
  }

  ret = listen(listenfd, LISTENQ);
  if (ret < 0)
  {
    perror("Failed to make a listening socket!\n"); return -1;
  }

	// set the global parameters
	g_tcpPort = port;
	g_tcpFd = listenfd;

  return listenfd;
}

/**
* @brief bindToUdpPort
*
* @param port
*
* @return file descriptor associated with
*					the open udp port. Returns -1 on
*					error.
*/
int bindToUdpPort(int port)
{
  // Variables.
  int socketId = 0;
  struct sockaddr_in serveraddr = {0};
  int ret = 0;
	int broadcastVal = 1;

  // Open the socket.
  socketId = socket(AF_INET, SOCK_DGRAM, 0);
  if ( socketId < 0 )
  {
    return -1;
  }

	// Make it so I can broadcast on this socket.
	ret = setsockopt(socketId, SOL_SOCKET, SO_BROADCAST, &broadcastVal, sizeof(broadcastVal) );
	if (ret == -1)
	{
		perror("setsockopt(SO_BROADCAST)");
		return -1;
	}

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(port);
  ret = bind(socketId, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));

  if (ret < 0)
  {
    fprintf(stderr, "Unable to bind to udp port %d!\n", port);
    return -1;
  }
  else
  {
#if DEBUG
    printf("Bound to udp port %d!\n", port);
#endif
  }

	// Set global port so that
	// other functions can send
	// data through this socket.
	g_udpPort = port;
	g_udpFd = socketId;
  return socketId;
}

void sendUpdatePacket(int udpFd, int *state)
{
	switch (*state)
	{
		case BROADCAST:
			sendBroadcast(udpFd);
		case SIMULATION:
			//sendHeartbeat(udpFd);
			;
	}
}

void sendBroadcast(int udpFd)
{
	broadCastPacket_t p;
	localHeadsetPos_t pl;
	getHeadsetPosData(&pl);
	p.packetType = BROADCASTPACKET;
	p.lat = pl.lat;
	p.lon = pl.lon;
	writeUdpByteStream((void *)&p, sizeof(p), 0);
}

void processPacket(int udpFd, int tcpFd, int ret, int pType, int *state)
{
	// If no packet was sent, then simply return immediately.
	if (ret == 0)
		return;

	// Process the type of packet.
	switch(pType)
	{
		case BROADCASTPACKET:
			getBroadCastPacket();
			break;
		case HEARTBEAT:
			getHeartBeatPacket();
			break;
		case CONFIRMUPDATE:
			break;
		default:
			break;
	}
	return;
}

int16_t writeTcpByteStream(void *buf, uint16_t size)
{
	return 0;
}

int16_t writeUdpByteStream(void *msg, uint16_t size, uint32_t dest)
{
  // Variables.
  int rc = 0;
	int bytesSent = 0;
  struct sockaddr_in clientaddr = {0};
  struct hostent *hp = NULL;

	// default to broadcast message.
	if (dest == 0)
	{
		dest = g_myBroadCast;
	}

  // Fill out connection structure.
  clientaddr.sin_port = htons(g_udpPort);
  clientaddr.sin_family = AF_INET;
  
	// Broadcast this message!
	clientaddr.sin_addr.s_addr = htonl(dest); // INADDR_BROADCAST

  // Connect to server
  bytesSent = sendto(g_udpFd, msg, size, 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr) );
  if ( bytesSent < 0 )
  {
    perror("Could not connect to server.\n");
    return -1;
  }
  else
  {
#if DEBUG
    printf("Sent broadcast to port %d.\n", port);
#endif
  }
  return bytesSent;
}

int16_t readTcpByteStream(void *buf, uint16_t size)
{
	return 0;
}
int16_t readUdpByteStream(void *buf, uint16_t size, uint16_t peek)
{
	return 0;
}

uint8_t detectUdpType(int fd)
{
	return 0;
}

uint8_t detectTcpType(int fd)
{
	return 0;
}

void getBroadCastPacket(void)
{
	/*
	broadCastPacket_t p;
	uint16_t i = 0, j = 0;
	// use BROADCASTPACKETSIZE-1 because
	// packetType was already read
	uint8_t buf[BROADCASTPACKETSIZE-1];
	readBytes(g_packetLibPort, buf, BROADCASTPACKETSIZE-1);
	broadCastPacketUnpack(&p,buf);
	if (findBroadCasting(p.address) < 0)
	{
		i = g_numBroadCasting;
		strncpy(g_broadCasting[i].address, p.address, SIZEOFID);
		g_numBroadCasting++;
    printf("\nProcessed broadcast packet num: %d\n", g_numBroadCasting);
		printf("lat: %f, lon: %f, id: ", p.lat, p.lon);
		for (j = 0; j < MAXSIZEOFID; j++)
		{
			printf("%2.2X ", p.address[j]);
		}
		printf("\n");
	}
	g_broadCasting[i].lat = p.lat;
	g_broadCasting[i].lon = p.lon;
	*/
	return;
}

void getHeartBeatPacket(void)
{
	/*
	heartBeat_t p;
	int16_t i = 0;
	// use HEARTBEATSIZE-1 because
	// packetType was already read
	uint8_t buf[HEARTBEATSIZE-1];
	readBytes(g_packetLibPort, buf, HEARTBEATSIZE-1);
	heartBeatUnpack(&p,buf);
	i = findHeartBeating(p.id);
	if (i >= 0)
	{
		g_heartBeating[i].x = p.x;
		g_heartBeating[i].y = p.y;
		g_heartBeating[i].roll = p.roll;
		g_heartBeating[i].pitch = p.pitch;
		g_heartBeating[i].yaw = p.yaw;
	}
	printf("x: %0.2f, y: %0.2f, y: %0.2f, p: %0.2f, r: %0.2f\n", p.x,p.y,p.roll,p.pitch,p.yaw);
	*/
	return;
}

// Function: printFloatBytes
// Purpose: Used to print the hex
// values of a floating point number
// as a sanity check that they are
// coming in correctly.
void printFloatBytes(char *buf)
{
  int i = 0;
  printf("Floating value:");
  for (i = 0; i < 4; i++)
  {
    printf(" [%d] = %X",i,buf[i]);
  }
  printf("\n");
}
