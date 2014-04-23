# This class allows me to create C structures
# in a seemless fasion so that I don't need to
# update multiple files and it allows me to create
# functions to get the "actual" size of the structure
# without padding. No duplicate files to update.

# Define constants to specify which
# c function to use for converting bytes.
NONE = 0;
LONG = 1;
SHORT = 2;
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
	def addMember(self,mtype, mname, msize, convFunc):
		# Tell user if there is an error in their type
		# Need better way to do this...., user could
		# define their own type and I assume pointers
		# shouldn't be counted as size of object....
		# Nested structures aren't very well supported.
		#if (mtype not in self.validTypes):
		#	raise Exception("Invalid type: %s" % (mtype, ) )
		if (convFunc == LONG):
			convFunc = ["htonl", "ntohl"]
		elif (convFunc == SHORT):
			convFunc = ["htons", "ntohs"]
		else:
			convFunc = ["",""]
		self.size = self.size + msize
		self.members.append((mtype, mname, convFunc))

	def __str__(self):
		retStr = self.comment
		retStr += "\ntypedef struct %s\n{\n" % (self.name, )
		for i in self.members:
			retStr = retStr + "\t%s\t%s;\n" % (i[0], i[1])
		retStr += "} %s_t;\n" % ( self.name, )
		return retStr
	def htonFunctionPrototype(self):
		retStr = "\nvoid\t%sHton(%s_t *p);" % (self.name, self.name )
		return retStr
	def ntohFunctionPrototype(self):
		retStr = "\nvoid\t%sNtoh(%s_t *p);" % (self.name, self.name )
		return retStr
	def htonFunction(self):
		retStr = "\nvoid\t%sHton(%s_t *p)\n{" % (self.name, self.name )
		for i in self.members:
			if (i[0] in self.validTypes):
				retStr += "\n\tp->%s = %s(p->%s);" % (i[1], i[2][0], i[1])

		retStr += "\n}\n"
		return retStr
	def ntohFunction(self):
		retStr = "\nvoid\t%sNtoh(%s_t *p)\n{" % (self.name, self.name )
		for i in self.members:
			# Updated to exclude packetType from unpacking...
			# packetType is usually already read when I want to unpack.
			if (i[0] in self.validTypes):
				retStr += "\n\tp->%s = %s(p->%s);" % (i[1], i[2][1], i[1])

		retStr += "\n\treturn;\n}\n"
		return retStr

# additional #defines shared
# between headset and ccu
addDefines = """
#define MAXNUMHEADSETS 10
"""

# Shared functions between headset and ccu
# These are miscelaneous functions.
addFuncProtStr = """

"""

addFuncDefStr = """

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
objInfo.addMember("uint32_t", "instId", 4, NONE) 	  # instance id of the object.
objInfo.addMember("uint32_t", "typeShow", 4, NONE)  # 0 (3d object hidden) 2 (3d object shown)
																								# 1 (2d object hidden) 3 (2d object shown)
objInfo.addMember("uint32_t", "x2", 4, SHORT)       # 2d coordinates of object.
objInfo.addMember("uint32_t", "y2", 4, SHORT)			
objInfo.addMember("float", "x3", 4, LONG)       		# 3d coordinates of object.
objInfo.addMember("float", "y3", 4, LONG)
objInfo.addMember("float", "z3", 4, LONG)
objInfo.addMember("float", "roll", 4, LONG)  			# orientation of object.
objInfo.addMember("float", "pitch", 4, LONG)
objInfo.addMember("float", "yaw", 4, LONG)
objInfo.addMember("float", "scale", 4, LONG)

broadCastPacketStr = """
// broadCastPacket: This is the packet
// that the headset will send repeatedly
// on startup. The ccu will listen for these
// packets to know how many headsets are
// available."""
broadCastPacket = Struct("broadCastPacket",broadCastPacketStr)
broadCastPacket.addMember("uint8_t", "packetType", 1, NONE)
broadCastPacket.addMember("float", "lat", 4, LONG)
broadCastPacket.addMember("float", "lon", 4, LONG)

acceptHeadsetStr = """
// AcceptHeadset:
// Sent from ccu to one headset.
// This tells the headset it is included and
// tells it where the geospatial origin is."""
acceptHeadset = Struct("acceptHeadset", acceptHeadsetStr)
acceptHeadset.addMember("uint8_t", "packetType", 1, NONE)
acceptHeadset.addMember("float", "x", 4, LONG)
acceptHeadset.addMember("float", "y", 4, LONG)

dropHeadsetStr = """
// DropHeadset:
// Sent from ccu to one headset.
// This tells the headset it is no longer 
// included in the simulation."""
dropHeadset = Struct("dropHeadset", dropHeadsetStr)
dropHeadset.addMember("uint8_t", "packetType", 1, NONE)

loadStaticDataStr = """
// LoadStaticData:
// Sent from ccu to all headsets.
// Sends a tar file which contains a group of 2d/3d objects
// and a configuration file which must be read that indicates
// the initial number and placement of objects as well as their id.
"""
loadStaticData = Struct("loadStaticData",loadStaticDataStr)
loadStaticData.addMember("uint8_t", "packetType", 1, NONE)
loadStaticData.addMember("uint32_t", "numBytes", 4, LONG)

updateObjInstanceStr = """
// UpdateObjInstance:
// Sent from ccu to headset to update the location
// of one or more objects."""
updateObjInstance = Struct("updateObjInstance", updateObjInstanceStr)
updateObjInstance.addMember("uint8_t", "packetType", 1, NONE)
updateObjInstance.addMember("uint8_t", "numObj", 1, NONE)
updateObjInstance.addMember("uint8_t", "updateNumber", 1, NONE)
updateObjInstance.addMember("objInfo_t", "*objList", 0, LONG)

startSimulationStr = """
// StartSimulation:
// Sent from ccu to one or more headsets
// This tells a headset to begin simulation."""
startSimulation = Struct("startSimulation", startSimulationStr)
startSimulation.addMember("uint8_t", "packetType", 1, NONE)

endSimulationStr = """
// EndSimulation:
// Sent from ccu to one or more headsets
// This tells a headset to end the simulation."""
endSimulation = Struct("endSimulation", endSimulationStr)
endSimulation.addMember("uint8_t", "packetType", 1, NONE)

heartBeatStr = """
// HeartBeat: This is the packet
// that the headset will send to the ccu
// during a simulation to inform the ccu
// about it's gps location and orientation."""
heartBeat = Struct("heartBeat", heartBeatStr)
heartBeat.addMember("uint8_t", "packetType", 1, NONE)
heartBeat.addMember("float", "x", 4, LONG)
heartBeat.addMember("float", "y", 4, LONG)
heartBeat.addMember("float", "lat", 4, LONG)
heartBeat.addMember("float", "lon", 4, LONG)
heartBeat.addMember("float", "roll", 4, LONG)
heartBeat.addMember("float", "pitch", 4, LONG)
heartBeat.addMember("float", "yaw", 4, LONG)

confirmPacketStr = """
// confirmPacket: This is the packet
// that the headset will send to indicate
// that it received an update from the ccu.
// Sent from headset directly to ccu."""
confirmPacket = Struct("confirmPacket", confirmPacketStr)
confirmPacket.addMember("uint8_t", "packetType", 1, NONE)
confirmPacket.addMember("uint8_t", "updateNumber", 1, NONE)

goBackStr = """
// Sent from ccu directly to a headset."""
goBack = Struct("goBack", goBackStr)
goBack.addMember("uint8_t", "packetType", 1, NONE)

packetList = []
packetList.append(objInfo)
packetList.append(broadCastPacket)
packetList.append(acceptHeadset)
packetList.append(dropHeadset)
packetList.append(loadStaticData)
packetList.append(updateObjInstance)
packetList.append(startSimulation)
packetList.append(endSimulation)
packetList.append(heartBeat)
packetList.append(confirmPacket)
