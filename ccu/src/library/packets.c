#define MAXNUMHEADSETS 10


// Local Function prototypes.

// Global Variables.
uint16_t g_numBroadCasting = 0;
broadCastInfo_t g_broadCasting[MAXNUMHEADSETS];
uint16_t g_numHeartBeating = 0;
heartBeatInfo_t g_heartBeating[MAXNUMHEADSETS];


// Function: calcCrc
// This function takes a list of characters
// which has probably been cast that way from a
// structure and performs a crc calculation.
uint16_t calcCrc(char *packet, int size)
{
  uint16_t ret = 0;
  // Make sure min size is one byte plus the 16bit crc
  if (packet != NULL && size > 1+sizeof(short))
  {
    ret = ( ((short)packet[0]) << 8) + packet[size-sizeof(short)];
    return ret;
  }
  return 0;
}

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
		pos->latitude = g_broadCasting[index].latitude;
		pos->longitude = g_broadCasting[index].longitude;
	}
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
		ids[i] = g_broadCasting[index].address;
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
}
// startSimulation()
int16_t startSimulation()
{
	startSimulation_t p = {0};
	p.packetType = STARTSIMULATION;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
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
	p.packetType = LOADSTATICDATA;
	// Get size of file.
	p.numBytes = ;
	// Contents of file. (just stream it byte by byte.)
	p.bytes = ;
	// Pack the packet to a byte stream.
	// Add header info and crc.
	// Write the packet to the serial port.
}
// updateObjs(objInfo *objList)
int16_t updateObjs(objInfo *objList, uint8_t numObjects)
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
}
// getAlive(id)
uint16_t getAlive(uint16_t id)
{
	int16_t ret = 0;
	ret = findHeartBeating(id);
	return (ret > 0 ? 1 : 0)
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
