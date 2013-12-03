# This class allows me to create C structures
# in a seemless fasion so that I don't need to
# update multiple files and it allows me to create
# functions to get the "actual" size of the structure
# without padding. No duplicate files to update.
class Struct:
	def __init__(self, name, comment):
		self.size = 0
		self.name = name
		self.comment = comment
		# This is a list of tuples with 2 elements ex:
		#      type       name       arrSize
		# [ ("uint8_t", "packetType", "0") ]
		self.members = []
		self.validTypes = ["uint8_t", "uint16_t", "uint32_t", "uint64_t", "float"]
	def addMember(self,mtype, mname, msize, arrSize):
		# Tell user if there is an error in their type
		# Need better way to do this...., user could
		# define their own type and I assume pointers
		# shouldn't be counted as size of object....
		# Nested structures aren't very well supported.
		#if (mtype not in self.validTypes):
		#	raise Exception("Invalid type: %s" % (mtype, ) )
		self.size = self.size + msize
		self.members.append((mtype, mname, arrSize))

	def __str__(self):
		retStr = self.comment
		retStr += "\ntypedef struct %s\n{\n" % (self.name, )
		for i in self.members:
			retStr = retStr + "\t%s\t%s;\n" % i
		retStr += "} %s_t;\n" % ( self.name, )
		return retStr
	def printSize(self):
		retStr = "\n#define %s %s" % (self.name.upper() + "SIZE", self.size)
		return retStr
	def packFunctionPrototype(self):
		retStr = "\nvoid\t%sPack(%s_t *p, uint8_t *buf);" % (self.name, self.name )
		return retStr
	def packFunction(self):
		retStr = "\nvoid\t%sPack(%s_t *p, uint8_t *buf)\n{" % (self.name, self.name )
		retStr += "\n\tuint32_t i = 0;\n"
		for i in self.members:
			if (i[0] in self.validTypes):
				if (i[2] == 0):
					retStr += "\n\t*((%s *)&buf[i]) = p->%s;" % (i[0], i[1])
					retStr += "\n\ti += sizeof(%s);" % (i[0], )
				else
					for j in range(i[2],0):
						retStr += "\n\t*((%s *)&buf[i]) = p->%s[j];" % (i[0], i[1])
						retStr += "\n\ti += sizeof(%s);" % (i[0], )

		retStr += "\n}\n"
		return retStr

# additional #defines shared
# between headset and ccu
addDefines = """
#define MAXNUMHEADSETS 10
#define HEADERSIZE 3
#define CRCSIZE 2
"""
# Shared functions between headset and ccu
addFuncProtStr = """
void addHeader(uint8_t *buf);
uint16_t calcCrc(char *packet, int size);"""

addFuncDefStr = """
void addHeader(uint8_t *buf)
{
  buf[0] = 'P';
  buf[1] = 'A';
  buf[2] = 'C';
  return;
}

// Function: calcCrc
// This function takes a list of characters
// which has probably been cast that way from a
// structure and performs a crc calculation.
uint16_t calcCrc(char *packet, int size)
{
  uint16_t ret = 0;
  // Make sure min size is one byte plus the 16bit crc
  if (packet != 0 /*NULL*/ && size > 1+sizeof(short))
  {
    ret = ( ((short)packet[0]) << 8) + packet[size-sizeof(short)];
    return ret;
  }
  return 0;
}
"""

# Packet definitions
typesEnum = """
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
};"""

objInfoStr = """
// Information for an object."""
objInfo = Struct("objInfo", objInfoStr)
objInfo.addMember("uint8_t", "instId", 1, 0) 	  # instance id of the object.
objInfo.addMember("uint8_t", "typeShow", 1, 0)  # 0 (3d object hidden) 2 (3d object shown)
																								# 1 (2d object hidden) 3 (2d object shown)
objInfo.addMember("uint16_t", "x2", 2, 0)       # 2d coordinates of object.
objInfo.addMember("uint16_t", "y2", 2, 0)			
objInfo.addMember("float", "x3", 4, 0)       		# 3d coordinates of object.
objInfo.addMember("float", "y3", 4, 0)
objInfo.addMember("float", "roll", 4, 0)  			# orientation of object.
objInfo.addMember("float", "pitch", 4, 0)
objInfo.addMember("float", "yaw", 4, 0)

broadCastPacketStr = """
// broadCastPacket: This is the packet
// that the headset will send repeatedly
// on startup. The ccu will listen for these
// packets to know how many headsets are
// available."""
broadCastPacket = Struct("broadCastPacket",broadCastPacketStr)
broadCastPacket.addMember("uint8_t", "packetType", 1, 0)
broadCastPacket.addMember("uint8_t", "address[16]", 16, 16)
broadCastPacket.addMember("float", "lattitude", 4, 0)
broadCastPacket.addMember("float", "longitude", 4, 0)

acceptHeadsetStr = """
// AcceptHeadset:
// Sent from ccu to one headset.
// This tells the headset it is included and
// tells it where the geospatial origin is."""
acceptHeadset = Struct("acceptHeadset", acceptHeadsetStr)
acceptHeadset.addMember("uint8_t", "packetType", 1, 0)
acceptHeadset.addMember("uint8_t", "id[16]", 16, 16)
acceptHeadset.addMember("float", "x", 4, 0)
acceptHeadset.addMember("float", "y", 4, 0)

loadStaticDataStr = """
// LoadStaticData:
// Sent from ccu to all headsets.
// Sends a tar file which contains a group of 2d/3d objects
// and a configuration file which must be read that indicates
// the initial number and placement of objects as well as their id.
"""
loadStaticData = Struct("loadStaticData",loadStaticDataStr)
loadStaticData.addMember("uint8_t", "packetType", 1, 0)
loadStaticData.addMember("uint32_t", "numBytes", 4, 0)

updateObjInstanceStr = """
// UpdateObjInstance:
// Sent from ccu to headset to update the location
// of one or more objects."""
updateObjInstance = Struct("updateObjInstance", updateObjInstanceStr)
updateObjInstance.addMember("uint8_t", "packetType", 1, 0)
updateObjInstance.addMember("uint8_t", "numObj", 1, 0)
updateObjInstance.addMember("uint8_t", "updateNumber", 1, 0)
updateObjInstance.addMember("objInfo_t", "*objList", 0, 0)

startSimulationStr = """
// StartSimulation:
// Sent from ccu to one or more headsets
// This tells a headset to begin simulation."""
startSimulation = Struct("startSimulation", startSimulationStr)
startSimulation.addMember("uint8_t", "packetType", 1, 0)

endSimulationStr = """
// EndSimulation:
// Sent from ccu to one or more headsets
// This tells a headset to end the simulation."""
endSimulation = Struct("endSimulation", endSimulationStr)
endSimulation.addMember("uint8_t", "packetType", 1, 0)

heartBeatStr = """
// HeartBeat: This is the packet
// that the headset will send to the ccu
// during a simulation to inform the ccu
// about it's gps location and orientation."""
heartBeat = Struct("heartBeat", heartBeatStr)
heartBeat.addMember("uint8_t", "packetType", 1, 0)
heartBeat.addMember("uint8_t", "id[8]", 8, 8) # lower 8 bytes of address.
heartBeat.addMember("float", "x", 4, 0)
heartBeat.addMember("float", "y", 4, 0)
heartBeat.addMember("float", "roll", 4, 0)
heartBeat.addMember("float", "pitch", 4, 0)
heartBeat.addMember("float", "yaw", 4, 0)

confirmPacketStr = """
// confirmPacket: This is the packet
// that the headset will send to indicate
// that it received an update from the ccu.
// Sent from headset directly to ccu."""
confirmPacket = Struct("confirmPacket", confirmPacketStr)
confirmPacket.addMember("uint8_t", "packetType", 1, 0)
confirmPacket.addMember("uint8_t", "updateNumber", 1, 0)

goBackStr = """
// Sent from ccu directly to a headset."""
goBack = Struct("goBack", goBackStr)
goBack.addMember("uint8_t", "packetType", 1, 0)

packetList = []
packetList.append(objInfo)
packetList.append(broadCastPacket)
packetList.append(acceptHeadset)
packetList.append(loadStaticData)
packetList.append(updateObjInstance)
packetList.append(startSimulation)
packetList.append(endSimulation)
packetList.append(heartBeat)
packetList.append(confirmPacket)
packetList.append(goBack)
