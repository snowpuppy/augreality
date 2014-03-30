#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <map>
#include <string>
#include "packets.h"
#include "packetLib.h"
#include "threadInterface.h"

// Constants
#define BAUDRATE B57600
#define XBEEPORT "/dev/ttyUSB0"

// Local Function prototypes.
int readBytes(int fd, char *data, int numBytes);

// Global Variables.
std::map<std::string, broadCastInfo_t> broadCastList;
std::map<std::string, heartBeatInfo_t> heartBeatList;
// 
int32_t g_packetLibPort;
int32_t g_udpPortSend = 0;
int32_t g_udpPortRecv = 0;
int32_t g_tcpPortRecv = 0;


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
    fprintf(stderr, "Could not open file: %s", filename);
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
  loadStaticDataPack(&p,&buf[HEADERSIZE]);
	// Add header info and crc.
  addHeader(buf);
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

// Function: findHeartBeating
// Purpose: abstract process of finding ids
// 			in the data structure they are in.
// Returns -1 on error
int16_t findHeartBeating(uint8_t *id)
{
	/*
	int16_t i = 0;
	for (i = 0; i < MAXNUMHEADSETS; i++)
	{
		if (strncmp(g_heartBeating[i].id, id, SIZEOFID/2) == 0 )
		{
			return i;
		}
	}
	return -1;
	*/
	return 0;
}

int openUdpPort()
{
	/*
	int res;
	struct termios tio;

	// Open serial port for reading/writing
	g_packetLibPort = open(XBEEPORT, O_RDWR|O_NOCTTY); 
	if (g_packetLibPort < 0)
	{
		perror(XBEEPORT);
		exit(1);
	}

	// Clear and then configure serial port
	bzero(&tio, sizeof(tio));
	tio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	// Set input mode (non-canonical, no echo,...)
	tio.c_lflag = 0;
	tio.c_cc[VTIME]    = 0;
	// Set number of characters to block until received
	tio.c_cc[VMIN]     = 1;

	// Flush the serial port
	tcflush(g_packetLibPort, TCIFLUSH);
	// Configure the serial port
	tcsetattr(g_packetLibPort,TCSANOW,&tio);

	return g_packetLibPort;
	*/
	return 0;
}

int16_t writeByteStream(uint8_t *buf, uint16_t size)
{
	/*
	return write(g_packetLibPort, (void *)buf, size);
	*/
	return 0;
}



// Function to read in a fixed number
// of bytes from the serial stream.
// User is responsible for pointer size.
int readBytes(int fd, char *data, int numBytes)
{
	/*
  int bytesRead = 0;
  int res = 0;
  //printf("Reading %d bytes.\n", numBytes);
  while (bytesRead < numBytes)
  {
    res = read(fd, data, numBytes - bytesRead);
    //printf("%d bytes read.\n", res);
    if (res < 0)
    {
      perror("Error reading serial data.\n");
    }
    else
    {
      // increment number of bytes read.
      // Keep reading till the expected number
      // of bytes is read.
      bytesRead += res;
    }
  }
	*/
	return 0;
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

uint8_t detectHeader(uint8_t *pac)
{
	/*
	uint8_t quit = 0;
	uint8_t ret = 0;
	while (!quit)
	{
		pac[2] = pac[1];
		pac[1] = pac[0];
		readBytes(g_packetLibPort,pac,1);
		if (pac[2] == 'P' && pac[1] == 'A' && pac[0] == 'C')
		{
			readBytes(g_packetLibPort,&ret,1);
			pac[0] = pac[1] = pac[2] = 0;
			return ret;
		}
		printf("\r%2.2X %2.2X %2.2X", pac[2], pac[1], pac[0]);
		usleep(20000);
	}
	*/
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
		printf("lat: %f, lon: %f, id: ", p.lattitude, p.longitude);
		for (j = 0; j < MAXSIZEOFID; j++)
		{
			printf("%2.2X ", p.address[j]);
		}
		printf("\n");
	}
	g_broadCasting[i].latitude = p.lattitude;
	g_broadCasting[i].longitude = p.longitude;
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
