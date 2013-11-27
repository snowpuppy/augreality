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
		# [ ("uint8_t", "packetType") ]
		self.members = []
		self.validTypes = ["uint8_t", "uint16_t", "uint32_t", "uint64_t", "float"]
	def addMember(self,mtype, mname, msize):
		# Tell user if there is an error in their type
		# Need better way to do this...., user could
		# define their own type and I assume pointers
		# shouldn't be counted as size of object....
		# Nested structures aren't very well supported.
		#if (mtype not in self.validTypes):
		#	raise Exception("Invalid type: %s" % (mtype, ) )
		self.size = self.size + msize
		self.members.append((mtype, mname))

	def __str__(self):
		retStr = self.comment
		retStr += "\ntypedef struct %s\n{\n" % (self.name, )
		for i in self.members:
			retStr = retStr + "\t%s\t%s;\n" % i
		retStr += "} %s_t;\n" % ( self.name, )
		return retStr
	def sizeFunctionPrototype(self):
		retStr = "\nuint32_t\t%sSize(void);" % (self.name, )
		return retStr
	def sizeFunction(self):
		retStr = "\nuint32_t\t%sSize(void)\n{" % (self.name, )
		retStr += "\n\treturn %s;" % (self.size, )
		retStr += "\n}\n"
		return retStr
	def packFunctionPrototype(self):
		retStr = "\nvoid\t%sPack(%s_t *p, uint8_t *buf);" % (self.name, self.name )
		return retStr
	def packFunction(self):
		retStr = "\nvoid\t%sPack(%s_t *p, uint8_t *buf)\n{" % (self.name, self.name )
		retStr += "\n\tuint32_t i = 0;\n"
		for i in self.members:
			if (i[0] in self.validTypes):
				retStr += "\n\t*((%s *)&buf[i]) = p->%s;" % (i[0], i[1])
				retStr += "\n\ti += sizeof(%s);" % (i[0], )
		retStr += "\n}\n"
		return retStr


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
objInfo.addMember("uint8_t", "instId", 1) 	# instance id of the object.
objInfo.addMember("uint8_t", "typeShow", 1) # 0 (3d object hidden) 2 (3d object shown)
																						# 1 (2d object hidden) 3 (2d object shown)
objInfo.addMember("uint16_t", "x2", 2)      # 2d coordinates of object.
objInfo.addMember("uint16_t", "y2", 2)			
objInfo.addMember("float", "x3", 4)         # 3d coordinates of object.
objInfo.addMember("float", "y3", 4)
objInfo.addMember("float", "roll", 4) 			# orientation of object.
objInfo.addMember("float", "pitch", 4)
objInfo.addMember("float", "yaw", 4)

broadCastPacketStr = """
// broadCastPacket: This is the packet
// that the headset will send repeatedly
// on startup. The ccu will listen for these
// packets to know how many headsets are
// available."""
broadCastPacket = Struct("broadCastPacket",broadCastPacketStr)
broadCastPacket.addMember("uint8_t", "packetType", 1)
broadCastPacket.addMember("uint16_t", "address", 2)
broadCastPacket.addMember("float", "lattitude", 4)
broadCastPacket.addMember("float", "longitude", 4)

acceptHeadsetStr = """
// AcceptHeadset:
// Sent from ccu to one headset.
// This tells the headset it is included and
// tells it where the geospatial origin is."""
acceptHeadset = Struct("accpetHeadset", acceptHeadsetStr)
acceptHeadset.addMember("uint8_t", "packetType", 1)
acceptHeadset.addMember("uint16_t", "id", 2)
acceptHeadset.addMember("float", "x", 4)
acceptHeadset.addMember("float", "y", 4)

loadStaticDataStr = """
// LoadStaticData:
// Sent from ccu to all headsets.
// Sends a tar file which contains a group of 2d/3d objects
// and a configuration file which must be read that indicates
// the initial number and placement of objects as well as their id.
"""
loadStaticData = Struct("loadStaticData",loadStaticDataStr)
loadStaticData.addMember("uint8_t", "packetType", 1)
loadStaticData.addMember("uint32_t", "numBytes", 4)

updateObjInstanceStr = """
// UpdateObjInstance:
// Sent from ccu to headset to update the location
// of one or more objects."""
updateObjInstance = Struct("updateObjInstance", updateObjInstanceStr)
updateObjInstance.addMember("uint8_t", "packetType", 1)
updateObjInstance.addMember("uint8_t", "numObj", 1)
updateObjInstance.addMember("uint8_t", "updateNumber", 1)
updateObjInstance.addMember("objInfo_t", "*objList", 0)

startSimulationStr = """
// StartSimulation:
// Sent from ccu to one or more headsets
// This tells a headset to begin simulation."""
startSimulation = Struct("startSimulation", startSimulationStr)
startSimulation.addMember("uint8_t", "packetType", 1)

endSimulationStr = """
// EndSimulation:
// Sent from ccu to one or more headsets
// This tells a headset to end the simulation."""
endSimulation = Struct("endSimulation", endSimulationStr)
endSimulation.addMember("uint8_t", "packetType", 1)

heartBeatStr = """
// HeartBeat: This is the packet
// that the headset will send to the ccu
// during a simulation to inform the ccu
// about it's gps location and orientation."""
heartBeat = Struct("heartBeat", heartBeatStr)
heartBeat.addMember("uint8_t", "packetType", 1)
heartBeat.addMember("uint16_t", "id", 2)
heartBeat.addMember("float", "x", 4)
heartBeat.addMember("float", "y", 4)
heartBeat.addMember("float", "roll", 4)
heartBeat.addMember("float", "pitch", 4)
heartBeat.addMember("float", "yaw", 4)

confirmPacketStr = """
// confirmPacket: This is the packet
// that the headset will send to indicate
// that it received an update from the ccu.
// Sent from headset directly to ccu."""
confirmPacket = Struct("confirmPacket", confirmPacketStr)
confirmPacket.addMember("uint8_t", "packetType", 1)
confirmPacket.addMember("uint8_t", "updateNumber", 1)

goBackStr = """
// Sent from ccu directly to a headset."""
goBack = Struct("goBack", goBackStr)
goBack.addMember("uint8_t", "packetType", 1)

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
