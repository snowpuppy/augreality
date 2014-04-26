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
#include <vector>
#include <string>
#include "packets.h"
#include "packetLib.h"
#include "threadInterface.h"
#include "gpsIMUDataThread.h"
#include <time.h> // time()
#include <string.h> // memcpy()

// Constants
#define BAUDRATE B57600
#define XBEEPORT "/dev/ttyUSB0"
#define LISTENQ 100
// update every 50 milliseconds (in useconds)
#define UPDATE_INTERVAL 50000
// define number of seconds before a headset
// is considered "dead"
#define SECONDS_TO_DIE 5

// Local Function prototypes.
//int readBytes(int fd, char *data, int numBytes);

// Global Variables (Maps ip addresses information.)
std::map<uint32_t, broadCastPacket_t> broadCastList;
std::map<uint32_t, heartBeatInfo_t> heartBeatList;
std::map<uint32_t, time_t> aliveList;
std::vector<objInfo_t> objectInfoList;
// 
int32_t g_packetLibPort;
int32_t g_udpPort = DEFAULT_UDP_PORT;
int32_t g_tcpPort = DEFAULT_TCP_PORT;
int32_t g_myIp = 0;
int32_t g_myMask = 0;
int32_t g_myBroadCast = 0;
int32_t g_udpFd = 0;
int32_t g_tcpFd = 0;
//int32_t g_state = INIT;
int32_t g_state = BROADCAST;
int32_t g_host = 0;

// API FUNCTIONS

// Function: getNumBroadCasting()
// Return the number of Headsets
// currently broadcasting.
uint16_t getNumBroadCasting()
{
	return broadCastList.size();
}

// Function: getBroadCastingIDs()
// Copies id numbers into the list and returns
// the number of values copied.
// Returns the number of IDs in the array, -1 on error.
int16_t getBroadCastingIDs(uint32_t *ids, uint16_t numIds)
{
	int i = 0;
	std::map<uint32_t, broadCastPacket_t>::iterator it;
	for (i=0,it = broadCastList.begin(); i < numIds && it != broadCastList.end(); i++, it++)
	{
		ids[i] = it->first;
	}
	return i;
}

// Function: acceptID(id)
int16_t acceptID(uint32_t destId)
{
	acceptHeadset p;
	uint32_t fd = 0;
	uint32_t rc = 0;
	uint32_t ret = 0;
	uint8_t buf[8];
	p.packetType = ACCEPTHEADSET;
	getGPSOrigin(&p.x, &p.y);
	// convert the accept packet to network
	// byte format.
	acceptHeadsetHton(&p);
	// Connect to the specified host.
	fd = connectToServer(g_tcpPort, destId);
	if (fd < 0)
	{
		perror("acceptID:Error connecting to server.\n");
		close(fd);
		return -1;
	}
	// write the type of packet to the wire.
	rc = write(fd, &p.packetType, sizeof(p.packetType));
	if (rc < 0)
	{
		perror("acceptID:Error writing to socket.\n");
		close(fd);
		return -1;
	}
	// write the accept packet to the wire.
	rc = write(fd, &p, sizeof(p));
	if (rc < 0)
	{
		perror("acceptID:Error writing to socket.\n");
		close(fd);
		return -1;
	}
	// read in two bytes to get response.
	rc = read(fd, buf, sizeof("ok"));
	if (rc < 0)
	{
		perror("acceptID:Error reading from socket.\n");
		close(fd);
		return -1;
	}
	// Check to see if we were able to accept the headset.
	if (strncmp((char *)buf,"ok",2) != 0)
	{
		perror("acceptID:Error did not receive ok!.\n");
		close(fd);
		return -1;
	}
	close(fd);
  return 0;
}

int16_t getAcceptID(int32_t connfd)
{
	int32_t rc = 0;
	uint8_t buf[] = "ok";
	acceptHeadset p;

	// Read the packet off of the wire. Note that we sent the id
	// twice so that this packet reading operation could work. I
	// understand that it is inefficient, but it is also easy to
	// fix and is currently more flexible.
	rc = read(connfd, &p, sizeof(p));
	if (rc < 0)
	{
		perror("getAcceptID: Could not read from file descriptor.");
		return -1;
	}
	acceptHeadsetNtoh(&p);
	setGPSOrigin(p.x,p.y);
	// respond with an ok.
	rc = write(connfd, buf, sizeof("ok"));
	if (rc < 0)
	{
		perror("getAcceptID: Could not write to file descriptor.");
		return -1;
	}
	return 0;
}

int16_t getDropId(int32_t connfd)
{
	dropHeadset_t p;
	int32_t rc = 0;
  uint32_t addr = 0;
  rc = readUdpByteStream((void *)&p, sizeof(p), &addr, 0);
	return rc;
}

int16_t sendDropId(uint32_t destId)
{
	dropHeadset_t p;
	int32_t rc = 0;
	p.packetType = DROPHEADSET;
	dropHeadsetHton(&p);
	rc = writeUdpByteStream((void *)&p, sizeof(p), destId);
  return rc;
}

// startSimulation()
int16_t startSimulation()
{
	startSimulation_t p;
	int32_t rc = 0;
	p.packetType = STARTSIMULATION;
	startSimulationHton(&p);
	rc = writeUdpByteStream((void *)&p, sizeof(p), 0);
  return rc;
}

int16_t getStartSimulation(int32_t connfd)
{
	startSimulation_t p;
	int32_t rc = 0;
  uint32_t addr = 0;
  rc = readUdpByteStream((void *)&p, sizeof(p), &addr, 0);
	return rc;
}

// endSimulationID(id)
int16_t endSimulationID(uint32_t destId)
{
	endSimulation_t p;
	int32_t rc = 0;
	p.packetType = ENDSIMULATION;
	endSimulationHton(&p);
	rc = writeUdpByteStream((void *)&p, sizeof(p), destId);
	return rc;
}

int16_t getEndSimulation(int32_t connfd)
{
	endSimulation_t p;
	int32_t rc = 0;
  uint32_t addr = 0;
  rc = readUdpByteStream((void *)&p, sizeof(p), &addr, 0);
	return rc;
}

// sendFile(filename)
int16_t sendFile(char *filename)
{
	loadStaticData_t p = {0};
  uint8_t buf[sizeof(loadStaticData_t)];
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
  write(g_packetLibPort, buf, sizeof(loadStaticData_t));
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
// Sends Five packets plus the header structure at a time until total number
// of packets have been sent.
// This is the most complicated send/receive function.
//
// 1. Convert all structures to network byte order
// 2. Bcopy them to to a character buffer
// 3. Send updates in groups of 5
int16_t updateObjs(objInfo_t *objList, uint32_t numObjects)
{
  updateObjInstance_t p;
  static uint32_t updateNum = 0;
  uint32_t offset = 0;
  int16_t ret = 0;
  uint32_t i = 0;
  uint8_t buf[sizeof(objInfo_t)*5+sizeof(updateObjInstance_t)] = {0};

  p.packetType = UPDATEOBJINSTANCE;
  p.numObj = 5;
  p.updateNumber = updateNum++;
  memcpy(buf, &p, sizeof(updateObjInstance_t));
  // Send all multiples of 5
  for (i = 0; i < numObjects/5; i++)
  {
    offset = sizeof(updateObjInstance_t);
    memcpy(buf+offset, &objList[i*5], sizeof(objInfo_t));
    offset += sizeof(objInfo_t);
    memcpy(buf+offset, &objList[i*5+1], sizeof(objInfo_t));
    offset += sizeof(objInfo_t);
    memcpy(buf+offset, &objList[i*5+2], sizeof(objInfo_t));
    offset += sizeof(objInfo_t);
    memcpy(buf+offset, &objList[i*5+3], sizeof(objInfo_t));
    offset += sizeof(objInfo_t);
    memcpy(buf+offset, &objList[i*5+4], sizeof(objInfo_t));

    // Write the five objects to neverland!
    ret = writeUdpByteStream((void *)buf, sizeof(buf), 0);
    if (ret < 0)
    {
      printf("Failed to send udp packet!\n");
      break;
    }
  }

  // Check if more updates to send.
  if ( (numObjects % 5) > 0)
  {
    // Send last group of packets.
    p.numObj = numObjects % 5;
    memcpy(buf, &p, sizeof(updateObjInstance_t));
    offset = sizeof(updateObjInstance_t);
    for (i = 0; i < numObjects % 5; i++)
    {
      memcpy(buf+offset, &objList[i], sizeof(objInfo_t));
      offset += sizeof(objInfo_t);
    }
    // Write the remaining objects to neverland!
    ret = writeUdpByteStream((void *)buf, offset, 0);
  }
  return 0;
}

int16_t getUpdateObjs()
{
  updateObjInstance_t p;
  objInfo_t objInfo = {0};
  static uint32_t updateNum = 0;
  uint32_t offset = 0;
  uint32_t addr = 0;
  uint32_t i = 0;
  uint8_t buf[sizeof(objInfo_t)*5+sizeof(updateObjInstance_t)] = {0};

  // read in the entire packet.
  readUdpByteStream(buf, sizeof(buf), &addr);

  // Copy information about update.
  memcpy(&p, buf, sizeof(updateObjInstance_t));
  // clear the list if we're receiving a new
  // update.
  if (p.updateNumber != updateNum)
  { 
    updateNum = p.updateNumber;
    objectInfoList.clear();
  }
  
  // Parse out the important details.
  offset = sizeof(updateObjInstance_t);
  // read in the five updates.
  for (i = 0; i < 5; i++)
  {
    memcpy(&objInfo, buf+offset, sizeof(objInfo_t));
    offset += sizeof(objInfo_t);
    objectInfoList.push_back(objInfo);
  }

  return 0;
}

// getAlive(id)
// Compare current time value
// to the last time a packet was
// received and determine if it is
// greater than a threshold.
uint16_t getAlive(uint32_t id)
{
  std::map<uint32_t, time_t>::iterator it;
  time_t timer = time(NULL);
  uint16_t ret = 0;
  it = aliveList.find(id);
  if (it != aliveList.end())
  {
    if (timer < it->second + SECONDS_TO_DIE)
    {
      ret = 1;
    }
  }
	return ret;
}

// getNumAlive()
uint16_t getNumAlive()
{
  uint32_t i = 0;
  time_t timer = time(NULL);
  std::map<uint32_t, time_t>::iterator it;
  for (it = aliveList.begin(); it != aliveList.end(); it++)
  {
    // Only return headsets that are still responding.
    if (timer < it->second + SECONDS_TO_DIE)
    {
      i++;
    }
  }
  // return actual number of ids.
	return i;
}

// getAliveIDs()
int16_t getAliveIDs(uint32_t *ids, uint16_t size)
{
  uint32_t i = 0;
  time_t timer = time(NULL);
  std::map<uint32_t, time_t>::iterator it;
  for (it = aliveList.begin(); it != aliveList.end() && i < size; it++)
  {
    // Only return headsets that are still responding.
    if (timer < it->second + SECONDS_TO_DIE)
    {
      ids[i] = it->first;
      i++;
    }
  }
  // return actual number of ids.
	return i;
}

// Function: getPos(id)
// Gets position information for headset with id.
// Returns -1 on error.
int16_t getPos(headsetPos_t *pos, uint32_t id)
{
  std::map<uint32_t, heartBeatInfo_t>::iterator it;
  it = heartBeatList.find(id);
  if (it == heartBeatList.end())
  {
    printf("Id %u not found in heartBeat list!\n", id);
    return -1;
  }
  pos->lat = (it->second).lat;
  pos->lon = (it->second).lon;
  pos->x = (it->second).x;
  pos->y = (it->second).y;
  pos->roll = (it->second).roll;
  pos->pitch = (it->second).pitch;
  pos->yaw = (it->second).yaw;
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

int connectToServer(int port, uint32_t addr)
{
  int fd = 0;
	int rc = 0;
  struct sockaddr_in sockAddr = {0};
	char paddr[INET_ADDRSTRLEN];
	uint32_t n_addr = htonl(addr);
	uint32_t n_myIp = htonl(g_myIp);

  // Fill out connection structure.
  sockAddr.sin_port = htons(port);
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_addr.s_addr = htonl(addr);

	inet_ntop(AF_INET, &n_addr, paddr, INET_ADDRSTRLEN);
	printf("Connecting to %s\n", paddr);
	inet_ntop(AF_INET, &n_myIp, paddr, INET_ADDRSTRLEN);
	printf("My Ip: %s\n", paddr);

	// Create a socket.
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if ( fd < 0 )
	{
		perror("Error creating socket!\n");
		return -1;
	}

	//Connect to server
	rc = connect(fd, (const struct sockaddr *) &sockAddr, sizeof(sockAddr) );
	if ( rc < 0 )
	{
		fprintf(stderr,"Error connecting to port %d!\n", port);
		return -1;
	}
	return fd;
}

void sendUpdatePacket(int udpFd)
{
	switch (g_state)
	{
		case BROADCAST:
			sendBroadcast(udpFd);
			break;
		case SIMULATION:
			sendHeartbeat(udpFd);
			break;
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
	broadCastPacketHton(&p);
	writeUdpByteStream((void *)&p, sizeof(p), 0);
}

void sendHeartbeat(int udpFd)
{
	heartBeat_t p;
	localHeadsetPos_t pl;
	getHeadsetPosData(&pl);
	p.packetType = HEARTBEAT;
	p.lat = pl.lat;
	p.lon = pl.lon;
	p.x = pl.x;
	p.y = pl.y;
	p.pitch = pl.pitch;
	p.yaw = pl.roll;
	p.roll = pl.yaw;
	heartBeatHton(&p);
	writeUdpByteStream((void *)&p, sizeof(p), 0);
}

/**
* @brief processPacket - reads from either the udp or tcp file descriptor
*				for a client connection and processes the packet. The type of packet
*				has been successfully determined if this function is called. Please
*				note that any tcp packet funcion called in here needs to close the
*				connFd file descriptor.
*
* @param udpFd - udp file descriptor.
* @param tcpFd - tcp file descriptor.
* @param connFd - tcp client connection file descriptor
* @param addr - the address of a client on a tcp connection only
* @param ret  - the file descriptor used to send the packet.
* @param pType - the type of packet received.
*/
void processPacket(int udpFd, int tcpFd, int connFd, uint32_t addr, int ret, int pType)
{
	int32_t rc = 0;
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
		case ACCEPTHEADSET:
			rc = getAcceptID(connFd);
      printf("Received Accept packet.\n");
			// Transition to next state.
			if (rc > 0 && g_state == BROADCAST)
			{
        printf("Transitioning to Accept state.\n");
				g_state = ACCEPTED;
			}
      printf("rc = %d, g_state = %d\n", rc, g_state);
			break;
    case DROPHEADSET:
      rc = getDropId(connFd);
      // Transition to next state
      if (rc > 0 && g_state != INIT)
      {
        g_state = INIT;
      }
    case UPDATEOBJINSTANCE:
      // only update objects if
      // running a simulation.
      if (g_state == SIMULATION)
      {
        rc = getUpdateObjs();
      }
		case STARTSIMULATION:
			getStartSimulation(connFd);
			if (g_state == ACCEPTED)
			{
				g_state = SIMULATION;
			}
			break;
		case ENDSIMULATION:
			getEndSimulation(connFd);
			if (g_state == SIMULATION)
			{
				g_state = INIT;
			}
			break;
		default:
			break;
	}
	return;
}

int32_t writeTcpByteStream(void *buf, uint32_t size)
{
	int32_t fd = 0, rc = 0;

	// Open connection.
	fd = connectToServer(g_tcpPort, g_myIp); // TODO: change from myIp!
	if (fd < 0)
	{
		perror("writeTcpByteStream:Error connecting to server.\n");
		return -1;
	}
	rc = write(fd, (void *)buf, size);
	if (rc < 0)
	{
		perror("writeTcpByteStream:Error writing to socket.\n");
		return -1;
	}
	printf("Sent tcp packet\n");
	// Close socket.
	close(fd);

	return rc;
}

int32_t writeUdpByteStream(void *msg, uint32_t size, uint32_t dest)
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

int32_t readTcpByteStream(void *buf, uint32_t size)
{
	uint32_t clientlen = 0, connfd = 0;
	struct sockaddr_in clientaddr = {0};
	int32_t rc = 0;

	connfd = accept(g_tcpFd, (struct sockaddr *)&clientaddr, &clientlen);

	rc = read(connfd, (void *)buf, size);
	if (rc < 0)
	{
		perror("readTcpByteStream: Could not read from file descriptor.");
	}
	close(connfd);

	return rc;
}

int32_t readUdpByteStream(void *buf, size_t size, uint32_t *addr, uint32_t peek)
{
	char paddr[INET_ADDRSTRLEN];
	int flags = 0;
	struct sockaddr src_addr = {0};
	socklen_t addrlen = sizeof(src_addr);
	int ret = 0;

	// Set the call to peek if the user requests
	// so that the next call will read the same
	// data.
	if (peek)
	{
		flags = MSG_PEEK;
	}

	// Retrieve data from the socket!
	ret = recvfrom(g_udpFd, buf, size, flags, &src_addr, &addrlen);
	if (ret < 0)
	{
		perror("Error receiving message!\n");
		return -1;
	}
	// Return the address associated with this
	// network packet....
	inet_ntop(AF_INET, &(((struct sockaddr_in *)&src_addr)->sin_addr), paddr, INET_ADDRSTRLEN);
	*addr = ntohl((((struct sockaddr_in *)&src_addr)->sin_addr.s_addr));
#if DEBUG
	printf("Received %d bytes from %s\n", ret, paddr);
#endif

	return ret;
}

uint8_t detectUdpType(int fd)
{
	uint8_t packetType = 0;
	uint32_t addr = 0;
	readUdpByteStream(&packetType, sizeof(packetType), &addr, 1);
	return packetType;
}

uint8_t detectTcpType(int fd, int *connfd, uint32_t *clientAddr)
{
	uint32_t clientlen = 0;
	struct sockaddr_in clientaddr = {0};
	int32_t rc = 0;
	uint8_t packetType = 0;

	// Accept the client connection.
	*connfd = accept(g_tcpFd, (struct sockaddr *)&clientaddr, &clientlen);
	if (*connfd < 0)
	{
		perror("Could not connect to host!\n");
		return -1;
	}
	// return the ip address of the client we connected to.
	*clientAddr = ntohl(clientaddr.sin_addr.s_addr);

	// Find out what type of packet was sent.
	rc = read(*connfd, (void *)&packetType, sizeof(packetType));
	if (rc < 0)
	{
		perror("detectTcpType: Could not read from file descriptor.");
		return -1;
	}

	return packetType;
}

void getBroadCastPacket(void)
{
	broadCastPacket_t p;
	uint32_t addr = 0;
	uint32_t ret = 0;

	// Read in the broadcast packet.
	ret = readUdpByteStream(&p, sizeof(broadCastPacket_t), &addr, 0);
	broadCastPacketNtoh(&p);
	if (ret > 0)
	{
		// update our entry for this address.
		broadCastList[addr] = p;
	}
	//printf("\nProcessed broadcast packet num: %8X\n", addr);
	//printf("lat: %f, lon: %f, id: ", p.lat, p.lon);
	return;
}

void getHeartBeatPacket(void)
{
	heartBeat_t p;
	heartBeatInfo_t hi;
	uint32_t addr = 0;
	uint32_t ret = 0;

	// Read in the heartbeat packet.
	ret = readUdpByteStream(&p, sizeof(heartBeat_t), &addr, 0);
	heartBeatNtoh(&p);
	if (ret > 0)
	{
		// update our entry for this address.
		// TODO: Add timestamp to packet.
		// Also only add this if the ip
		// has been previously accepted.
		hi.lat = p.lat;
		hi.lon = p.lon;
		hi.x = p.x;
		hi.y = p.y;
		hi.pitch = p.pitch;
		hi.yaw = p.yaw;
		hi.roll = p.roll;
		hi.lastUpdate = time(NULL);
		hi.ipAddr = addr;
		heartBeatList[addr] = hi;
    aliveList[addr] = time(NULL);
	}
	//printf("\nProcessed heartbeat packet num: %8X\n", addr);
	//printf("lat: %f, lon: %f, id: ", p.lat, p.lon);
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

uint32_t getMyId()
{
  return g_myIp;
}

uint32_t getState()
{
  return g_state;
}

uint32_t setHostHeadset(int32_t host)
{
  g_host = host;
  if (!host)
  {
    g_state = BROADCAST;
  }
}
