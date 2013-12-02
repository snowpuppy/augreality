#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "packets.h"
#include "packetLib.h"

// Constants
#define BAUDRATE B57600
#define XBEEPORT "/dev/ttyUSB0"

// Local Function prototypes.

// Global Variables.
uint16_t g_numBroadCasting = 0;
broadCastInfo_t g_broadCasting[MAXNUMHEADSETS];
uint16_t g_numHeartBeating = 0;
heartBeatInfo_t g_heartBeating[MAXNUMHEADSETS];
int32_t g_port;


// API FUNCTIONS

// Function: getBroadCastingLoc(id)
// Returns an object for a given id that specifies
// the location information for that object.
int16_t getBroadCastingLoc(headsetPos_t *pos, uint16_t id)
{
	int16_t index = -1;
	index = findBroadCasting(id);
	if (index > 0)
	{
		pos->x = g_broadCasting[index].latitude;
		pos->y = g_broadCasting[index].longitude;
	}
  return 0;
}

// Function: getNumBroadCasting()
// Return the number of Headsets
// currently broadcasting.
uint16_t getNumBroadCasting()
{
	return g_numBroadCasting;
}

// Function: getBroadCastingIDs()
// Copies id numbers into the list and returns
// the number of values copied.
// Returns the number of IDs in the array, -1 on error.
int16_t getBroadCastingIDs(uint16_t *ids, uint16_t size)
{
	int16_t i = 0;
	for (i = 0; i < size && i < g_numBroadCasting; i++)
	{
		ids[i] = g_broadCasting[i].address;
	}
	return i;
}

// Function: acceptID(id)
int16_t acceptID(uint16_t ccuId, uint16_t destId, float originLat, float originLon )
{
	// Create a packet and stuff it.
	accpetHeadset_t p = {0};
	p.packetType = BROADCASTPACKET;
	p.id = ccuId;
	p.x = originLat;
	p.y = originLon;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
  return 0;
}
// startSimulation()
int16_t startSimulation()
{
	startSimulation_t p = {0};
	p.packetType = STARTSIMULATION;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
  return 0;
}
// endSimulationID(id)
int16_t endSimulationID(uint16_t destId)
{
	endSimulation_t p = {0};
	p.packetType = ENDSIMULATION;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
}
// sendFile(filename)
int16_t sendFile(char *filename)
{
	loadStaticData_t p = {0};
  uint8_t buf[LOADSTATICDATASIZE + HEADERSIZE + CRCSIZE];
  uint8_t fileBuf[256];
  uint16_t bytesRead = 0;
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
  write(g_port, buf, LOADSTATICDATASIZE + HEADERSIZE);
  // Write the file to the serial port
  while ( !feof(fp))
  {
    bytesRead = fread(fileBuf,1,256,fp);
    write(g_port, fileBuf, bytesRead);
  }
  return 0;
}
// updateObjs(objInfo *objList)
int16_t updateObjs(objInfo_t *objList, uint8_t numObjects)
{
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
  return 0;
}
// getAlive(id)
uint16_t getAlive(uint16_t id)
{
	int16_t ret = 0;
	ret = findHeartBeating(id);
	return (ret > 0 ? 1 : 0);
}
// getNumAlive()
uint16_t getNumAlive()
{
	return g_numHeartBeating;
}
// getAliveIDs()
int16_t getAliveIDs(uint16_t *ids, uint16_t size)
{
	int16_t i = 0;
	for (i = 0; i < g_numHeartBeating && i < size; i++)
	{
		ids[i] = g_heartBeating[i].id;
	}
	return i;
}
// Function: getPos(id)
// Sets position information for headset with id.
// Returns -1 on error.
int16_t getPos(headsetPos_t *pos, uint16_t id)
{
	int16_t index = 0;
	if (pos == NULL)
	{
		return -1;
	}
	index = findHeartBeating(id);
	if (index > 0)
	{
		pos->x = g_heartBeating[index].x;
		pos->y = g_heartBeating[index].y;
		pos->roll = g_heartBeating[index].roll;
		pos->pitch = g_heartBeating[index].pitch;
		pos->yaw = g_heartBeating[index].yaw;
	}
	return index;
}
// Function: goBack(id)
// Send a goBack message to the headset
// indicating that it should transition to
// an earlier state.
int16_t goBack(uint16_t id)
{
	goBack_t p = {0};
	p.packetType = GOBACK;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
  return 0;
}


// NON_API FUNCTIONS

// Function: findBroadCastingLoc
// Purpose: abstract process of finding ids
// 			in the data structure they are in.
// Returns -1 on error
int16_t findBroadCasting(uint16_t id)
{
	int16_t i = 0;
	for (i = 0; i < MAXNUMHEADSETS; i++)
	{
		if (g_broadCasting[i].address == id)
		{
			return i;
		}
	}
	return -1;
}

// Function: findHeartBeating
// Purpose: abstract process of finding ids
// 			in the data structure they are in.
// Returns -1 on error
int16_t findHeartBeating(uint32_t id)
{
	int16_t i = 0;
	for (i = 0; i < MAXNUMHEADSETS; i++)
	{
		if (g_heartBeating[i].id == id)
		{
			return i;
		}
	}
	return -1;
}

int openComPort()
{
	int res;
	struct termios tio;

	// Open serial port for reading/writing
	g_port = open(XBEEPORT, O_RDWR|O_NOCTTY); 
	if (g_port < 0)
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
	tcflush(g_port, TCIFLUSH);
	// Configure the serial port
	tcsetattr(g_port,TCSANOW,&tio);

	return g_port;
}
