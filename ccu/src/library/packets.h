// File: packet.h
// Contains the definition of the
// packets exchanged wirelessly. The
// header and crc data will be added later.
#ifndef PACKET_H
#define PACKET_H


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

// Information for an object.
typedef struct objInfo
{
    uint8_t instId;         // instance id of the object.
    uint8_t typeShow;       // Type: 2d or 3d object, Show: show or hide.
                            // 0 (3d object hidden) 2 (3d object shown)
                            // 1 (2d object hidden) 3 (2d object shown)
    float   x3,y3;            // 3d coordinates of object.
    int     x2,y2;            // 2d coordinates of object.
    float   roll,pitch,yaw; // orientation of object.
} objInfo_t;

// PACKETS
enum packetType
{
	BROADCASTPACKET = 1,
	ACCEPTHEADSET,
	LOADSTATICDATA,
	UPDATEOBJINSTANCE,
	STARTSIMULATION,
	ENDSIMULATION,
	HEARTBEAT,
	CONFIRMUPDATE,
	GOBACK
};

// broadCastPacket: This is the packet
// that the headset will send repeatedly
// on startup. The ccu will listen for these
// packets to know how many headsets are
// available.
typedef struct broadCastPacket
{
  char type;        // type of packet to be sent
  uint16_t address; // network address of xbee
  float latitude;   // latitude of headset
  float longitude;  // longitude of headset
} broadCastPacket_t;

// AcceptHeadset:
// Sent from ccu to one headset.
// This tells the headset it is included and
// tells it where the geospatial origin is.
typedef struct accpetHeadset
{
    uint8_t packetType; // type of packet being sent.
    uint16_t id;        // id of ccu.
    float x,y;          // location of origin.
} accpetHeadset_t;

// LoadStaticData:
// Sent from ccu to all headsets.
// Sends a tar file which contains a group of 2d/3d objects
// and a configuration file which must be read that indicates
// the initial number and placement of objects as well as their id.
typedef struct loadStaticData
{
    uint8_t packetType; // type of packet being sent.
    uint32_t numBytes;   // number of bytes in packet.
} loadStaticData_t;

// UpdateObjInstance:
// Sent from ccu to headset to update the location
// of one or more objects.
typedef struct updateObjInstance
{
    uint8_t packetType;   // type of packet being sent
    uint8_t numObj;       // number of objects to update
    uint8_t updateNumber; // id of update packet (used for confirmation)
    objInfo_t *objList;   // list of objects to update
} updateObjInstance_t;

// StartSimulation:
// Sent from ccu to one or more headsets
// This tells a headset to begin simulation.
typedef struct startSimulation
{
    uint8_t packetType; // type of packet being sent
} startSimulation_t;

// EndSimulation:
// Sent from ccu to one or more headsets
// This tells a headset to end the simulation.
typedef struct endSimulation
{
    uint8_t packetType; // type of packet being sent
} endSimulation_t;

// HeartBeat: This is the packet
// that the headset will send to the ccu
// during a simulation to inform the ccu
// about it's gps location and orientation.
typedef struct heartBeat
{
    uint8_t packetType;     // type of packet being sent.
    uint16_t id;            // id of the player (same as xbee address)
    float x,y;              // coordinates of player
    float roll,pitch,yaw;   // orientation of player
} heartBeat_t;

// confirmPacket: This is the packet
// that the headset will send to indicate
// that it received an update from the ccu.
// Sent from headset directly to ccu.
typedef struct confirmUpdate
{
    uint8_t packetType;
    uint8_t updateNumber;
} confirmUpdate_t;

// Sent from ccu directly to a headset.
typedef struct goBack
{
    uint8_t packetType;
} goBack_t;

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
