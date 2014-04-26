# Filename: guiNetInterface.py
# Author: Thor Smith
# Purpose: Create functions for performing socket calls
#          to abstract the process of getting information
#

HOST = ''   # Symbolic name meaning the local host
PORT = 7777 # Arbitrary non-privileged port

from struct import *
import socket

GETUSERPOSITION = '\x01'
GETWIFISTATUS = '\x02'
GETBATTERYSTATUS = '\x03'
GETUSERPOSITIONFORMATS = '=B'
GETUSERPOSITIONFORMATR = '=I7f'
GETBROADCASTIDS = '\x04'
BROADCASTIDSFORMATS = '=B'
GETNUMBROADCAST = '\x05'
NUMBROADCASTFORMATS = '=B'
NUMBROADCASTFORMATR = '=B'
GETBROADCASTLOC = '\x06'
BROADCASTLOCFORMATS = '=B16B'
BROADCASTLOCFORMATR = '2f3f' # x,y,roll,pitch,yaw
GETPOSITION = '\x07'
POSITIONFORMATS = '=B16B'
POSITIONFORMATR = '7f'
GETNUMALIVE = '\x08'
NUMALIVEFORMATS = '=B'
NUMALIVEFORMATR = '=B'
GETALIVE = '\x09'
ALIVEFORMATS = '=B16B'
ALIVEFORMATR = '=B'
SENDUPDATEOBJS = '\x0a'
SENDUPDATEOBJSFORMAT = '=BB'
OBJSFORMAT = 'BBHH5f'			# objInfo struct
SENDFILE = '\x0b'
SENDFILEFORMAT = '=B'		# also need to send filename
SENDEND = '\x0c'
SENDENDFORMAT = '=B16B'
SENDSTART = '\x0d'
SENDSTARTFORMAT = '=B'
SENDACCEPT = '\x0e'
SENDRESETORIGIN = '\x0f'
SETGPSORIGIN = '\x10'
SETGPSORIGINFORMAT = '=2f'
GETGPSORIGIN = '\x11'
GETGPSORIGINFORMAT = '=2f'
GETACCEPTEDIDS = '\x12'
GETRECEIVEDFILE = '\x13'
GETEND = '\x14'
GETSTART = '\x15'
GETDROP = '\x16'
SENDDROP = '\x17'
GETACCEPT = '\x18'
SETHOSTHEADSET = '\x19'
GETMYID = '\x20'
GETUPDATEOBJS = '\x21'
NIDFORMAT = '=I'
IDLISTFORMAT = '=%sI'

##
# @brief getBroadCastIDs() gets a list of 16 byte
#   strings indicating the id of a headset in hex.
#
# @return a list of 16 byte strings
def getBroadCastIDs():
	# Set command
	# Pack info
	command = GETBROADCASTIDS
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# receive info
	# Get number of ids to read.
	numIds = s.recv(1)
	data = ""
	if (ord(numIds) > 0):
		#print (IDLISTFORMAT % (ord(numIds)*16,));
		numToRead = calcsize(IDLISTFORMAT % (ord(numIds),))
		#print "numToRead:",numToRead
		reply = s.recv(numToRead);
		data = unpack(IDLISTFORMAT % (ord(numIds),),reply);
	#print "NumIds:",ord(numIds)
	#print "IDs: ",data
	s.close()
	output = []
	# Format the data, then return it.
	# Returns a list of integers.
	return data

##
# @brief getNumBroadCast() gets the number of headsets
#					that are currently broadcasting.
#
# @return integer indicating number of headsets
def getNumBroadCast():
	# Set command
	# Pack info
	command = GETNUMBROADCAST
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# receive info
	reply = s.recv(1);
	#print "NumIds:",ord(reply)
	s.close()
	return ord(reply)

##
# @brief getPostion() gets the position information
#				for a specific headset. Meant to be used during
#				simulation runtime.
#
# @param nid - id of headset to get info from.
#
# @return a tuple containing x,y,pitch,yaw,roll.
def getPosition(nid):
	# Set command
	# Pack info
	command = GETPOSITION
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# send id
	data = pack(NIDFORMAT, nid)
	s.send(data)
	# receive info
	data = ""
	# Get number of bytes to read.
	numToRead = calcsize( POSITIONFORMATR )
	#print "numToRead:", numToRead
	reply = s.recv(numToRead);
	s.close()
	data = unpack(POSITIONFORMATR,reply);
	#print "Position: ",data
	# returns lat,lon,x,y,roll,pitch,yaw
	return data

##
# @brief getUserPostion() gets the position information
#				for this headset . Meant to be used during
#				simulation runtime.
#
# @return a tuple containing numSat,lat,lon,x,y,roll,pitch,yaw
def getUserPosition():
	# Set command
	# Pack info
	command = GETUSERPOSITION
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# receive info
	data = ""
	# Get number of bytes to read.
	numToRead = calcsize( GETUSERPOSITIONFORMATR )
	#print "numToRead:", numToRead
	reply = s.recv(numToRead);
	s.close()
	data = unpack(GETUSERPOSITIONFORMATR,reply);
	#print "Position: ",data
	# returns numSat,lat,lon,x,y,roll,pitch,yaw
	return data

##
# @brief getNumAlive()
#
# @return - integer indicating the number of headsets
#						that are available wirelessly.
def getNumAlive():
	# Set command
	# Pack info
	command = GETNUMALIVE
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# receive info
	reply = s.recv(1);
	#print "NumAlive:",ord(reply)
	s.close()
	return ord(reply)

##
# @brief getAlive() determins if a headset is
#				 still connected wirelessly to the central
#				 device.
#
# @param nid - idea of headset to probe
#
# @return - 1 for connected, 0 for disconnected
def getAlive(nid):
	# Set command
	# Pack info
	command = GETALIVE
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# send id
	data = pack(NIDFORMAT, nid)
	s.send(data)
	# receive info
	# Get number of ids to read.
	data = ""
	reply = s.recv(1);
	#print "Alive: ", ord(reply)
	s.close()
	# returns 1 for alive, 0 for disconnected
	return ord(reply)

##
# @brief sendFile() sends a file that sets the simulation
#			   environment.
#
# @param filename - string indicating the file to be sent.
#
# @return - none
def sendFile(filename,nid):
	# Set command
	# Pack info
	command = SENDFILE
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# send the id that it goes to.
	data = pack(NIDFORMAT, nid)
	s.send(data)
	# send filename length
	s.send(chr(len(filename)))
	# send filename
	s.send(filename)
	s.close()

def sendUpdateObjs(num,instId,typeShow,x2,y2,x3,y3,roll,pitch,yaw):
	# Set command
	# format SENDUPDATEOBJSFORMAT'=BB' 
	# format OBJSFORMAT 'BBHH5f'
	# Pack info
	command = SENDUPDATEOBJS
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	s.send(chr(num))
	data = pack(OBJSFORMAT, instId, typeShow, x2, y2, x3, y3, roll, pitch, yaw)
	s.send(data)
	s.close()
	
def sendEnd(nid):
	# Set command
	# Pack info
	command = SENDEND
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# send id
	data = pack(NIDFORMAT, nid)
	s.send(data)
	s.close()

def sendStart():
	# Set command
	# Pack info
	command = SENDSTART
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	s.close()

def sendAccept(nid):
	# Set command
	# Pack info
	command = SENDACCEPT
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# send id
	data = pack(NIDFORMAT, nid)
	s.send(data)
	reply = s.recv(1)
	s.close()
	data = ord(reply)
	return data

def resetGPSOrigin():
	# Set command
	# Pack info
	command = SENDRESETORIGIN
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	s.close()

def setGPSOrigin(lat,lon):
	# Set command
	# Pack info
	command = SETGPSORIGIN
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# pack and send gps coordinates
	data = pack(SETGPSORIGINFORMAT, lat, lon)
	s.send(data)
	s.close()

def getGPSOrigin():
	# Set command
	# Pack info
	command = GETGPSORIGIN
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	numToRead = calcsize(GETGPSORIGINFORMAT)
	reply = s.recv(numToRead)
	s.close()
	data = unpack(GETGPSORIGINFORMAT, reply)
	return data

def getAcceptIds():
	# Set command
	# Pack info
	command = GETACCEPTEDIDS
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# Get number of ids to read.
	numIds = s.recv(1)
	data = ""
	if (ord(numIds) > 0):
		#print (IDLISTFORMAT % (ord(numIds)*16,));
		numToRead = calcsize(IDLISTFORMAT % (ord(numIds),))
		#print "numToRead:",numToRead
		reply = s.recv(numToRead);
		data = unpack(IDLISTFORMAT % (ord(numIds),),reply);
	s.close()
	output = []
	# Format the data, then return it.
	# Returns a list of integers.
	return data

def getReceivedFile():
	# Set command
	# Pack info
	filename = ""
	command = GETRECEIVEDFILE
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# receive status
	reply = s.recv(1)
	received = ord(reply)
	if (received):
		reply = s.recv(1)
		filename = s.recv(ord(reply))
	return str(filename)
	s.close()

def getEnd():
	# Set command
	# Pack info
	command = GETEND
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	reply = s.recv(1)
	s.close()
	data = ord(reply)
	return data

def getStart():
	# Set command
	# Pack info
	command = GETSTART
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	reply = s.recv(1)
	s.close()
	data = ord(reply)
	return data

def getDrop():
	# Set command
	# Pack info
	command = GETDROP
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	reply = s.recv(1)
	s.close()
	data = ord(reply)
	return data

def sendDrop(nid):
	# Set command
	# Pack info
	command = SENDDROP
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	# send id
	data = pack(NIDFORMAT, nid)
	s.send(data)
	s.close()

def getAccept():
	# Set command
	# Pack info
	command = GETACCEPT
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	reply = s.recv(1)
	s.close()
	data = ord(reply)
	return data

def setHostHeadset(host):
	command = SETHOSTHEADSET
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	s.send(chr(host))
	s.close()

def getMyId():
	command = GETMYID
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	numToRead = calcsize(NIDFORMAT)
	reply = s.recv(numToRead)
	data = unpack(NIDFORMAT, reply)
	s.close()

def getWiFiStatus():
	# Set command
	# Pack info
	command = GETWIFISTATUS
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	reply = s.recv(1)
	s.close()
	data = ord(reply)
	return data

def getWiFiStatus():
	# Set command
	# Pack info
	command = GETBATTERYSTATUS
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	reply = s.recv(1)
	s.close()
	data = ord(reply)
	return data

def getUpdateObjs():
	# Set command
	# Pack info
	command = GETBATTERYSTATUS
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	s.close()
	return
