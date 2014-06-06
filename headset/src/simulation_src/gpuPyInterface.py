# Filename: guiNetInterface.py
# Author: Thor Smith
# Purpose: Create functions for performing socket calls
#          to abstract the process of getting information
#

HOST = ''   # Symbolic name meaning the local host
PORT = 7778 # Arbitrary non-privileged port
NETPORT = 7777

from struct import *
import socket

GETRUNNING = '\x01'
GETRUNNINGFORMATS = '=B'
GETRUNNINGFORMATR = '=B'
GPUQUIT = '\x02'
GPUQUITFORMATS = '=B'
GPUQUITFORMATR = '=I7f'
SENDUPDATEOJBSGPU = '\x03'
SENDUPDATEOJBSGPUFORMAT = '=BB'
OBJSGPUFORMAT = 'IIII7f'			# objInfo struct

# Net commands
SENDUPDATEOBJS = '\x0a'
SENDUPDATEOBJSFORMAT = '=BB'
GETUPDATEOBJS = '\x1b'

##
# @brief ObjInfo_t mirros a c structure. This structure is used to bridge the gap between
#					C and python code and is used to pack information when sending info through sockets.
class ObjInfo_t:
	def __init__(self, instId, typeShow, x2, y2, x3, y3, z3, roll, pitch, yaw, scale, name):
		self.instId = int(instId)
		self.typeShow = int(typeShow)
		self.x2 = float(x2)
		self.y2 = float(y2)
		self.x3 = float(x3)
		self.y3 = float(y3)
		self.z3 = float(z3)
		self.roll = float(roll)
		self.pitch = float(pitch)
		self.yaw = float(yaw)
		self.scale = float(scale)
		self.name = str(name)
		self.state = 0
	def __str__(self):
		return "%d %d %f %f %f %f %f %f %f %f %f %s" % (self.instId, self.typeShow, self.x2, self.y2, self.x3, self.y3, self.z3, self.roll, self.pitch, self.yaw, self.scale, self.name)

##
# @brief readObjsFromConfig - reads in the objects from the file and puts the
#					information into an object with named parameters.
#
# @param fp - open file descriptor for the file to be read from.
#
# @return - returns a dictionary categorized by the name of the object.
#						Each dictionary entry is a list of the objects with the given name.
def readObjsFromConfig(fp):
	myLines = fp.readlines()
	myObjsDict = {}
	for i in range(0,len(myLines),2):
		instId = int(myLines[i].strip())
		data = myLines[i+1].strip().split()
		name = data[7].split("/")[-1]
		if name not in myObjsDict:
			myObjsDict[name] = []
		myObjsDict[name].append(ObjInfo_t(instId,data[8],0,0,data[1],data[2],data[3],data[4],data[5],data[6],data[9],name))
	return myObjsDict

##
# @brief getPostion() gets the position information
#				for a specific headset. Meant to be used during
#				simulation runtime.
#
# @param nid - id of headset to get info from.
#
# @return a tuple containing x,y,pitch,yaw,roll.
def getRunning():
	# Set command
	# Pack info
	command = GETRUNNING
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# receive info
	data = ""
	# Get number of bytes to read.
	numToRead = calcsize( GETRUNNINGFORMATS )
	#print "numToRead:", numToRead
	reply = s.recv(numToRead);
	s.close()
	data = unpack(GETRUNNINGFORMATR,reply);
	#print "Position: ",data
	# returns x,y,roll,pitch,yaw
	return data

##
# @brief getUserPostion() gets the position information
#				for this headset . Meant to be used during
#				simulation runtime.
#
# @return a tuple containing numSat,lat,lon,x,y,roll,pitch,yaw
def gpuQuit():
	# Set command
	# Pack info
	command = GPUQUIT
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	s.close()
	return

##
# @brief sendUpdateObjsGpu
#
# @param num - the number of objects to update
# @param objs - a list of objInfo_t for the objects to update
#
# @return 
def sendUpdateObjsGpu(num,objs):
	# Set command
	# format SENDUPDATEOJBSGPUFORMAT'=BB' 
	# format OBJSGPUFORMAT 'IIII7f'
	# Pack info
	command = SENDUPDATEOJBSGPU
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	data = pack('I',num)
	s.send(data)
	for i in objs:
		data = pack(OBJSGPUFORMAT, i.instId, i.typeShow, i.x2, i.y2, i.x3, i.y3, i.z3, i.roll, i.pitch, i.yaw, i.scale)
		s.send(data)
	s.close()
	return


def sendUpdateObjs(num,objs):
	# Set command
	# format SENDUPDATEOJBSGPUFORMAT'=BB' 
	# format OBJSGPUFORMAT 'IIII7f'
	# Pack info
	command = SENDUPDATEOBJS
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,NETPORT))
	# send info
	s.send(command)
	data = pack('I',num)
	s.send(data)
	for i in objs:
		data = pack(OBJSGPUFORMAT, i.instId, i.typeShow, i.x2, i.y2, i.x3, i.y3, i.z3, i.roll, i.pitch, i.yaw, i.scale)
		s.send(data)
	s.close()
	return
	
def getUpdateObjs():
	# Set command
	# Pack info
	command = GETUPDATEOBJS
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,NETPORT))
	# send info
	s.send(command)
	numToRead = calcsize('I')
	reply = s.recv(numToRead)
	numObjs = unpack('I',reply)[0]
	objs = []
	numToRead = calcsize(OBJSGPUFORMAT)
	if numObjs > 0:
		for i in range(numObjs):
			reply = s.recv(numToRead)
			data = unpack(OBJSGPUFORMAT, reply)
			obj = ObjInfo_t(data[0], data[1], data[2], data[3], data[4],data[5], data[6], data[7], data[8], data[9], data[10], 'unknown')
			objs.append(obj)

	s.close()
	return objs
