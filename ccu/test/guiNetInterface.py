# Filename: guiNetInterface.py
# Author: Thor Smith
# Purpose: Create functions for performing socket calls
#          to abstract the process of getting information
#

HOST = ''   # Symbolic name meaning the local host
PORT = 7777 # Arbitrary non-privileged port

from struct import *
import socket

GETBROADCASTIDS = '\x01'
BROADCASTIDSFORMATS = '=B'
BROADCASTIDSFORMATR = '=%sB'
GETNUMBROADCAST = '\x02'
NUMBROADCASTFORMATS = '=B'
NUMBROADCASTFORMATR = '=B'
GETBROADCASTLOC = '\x03'
BROADCASTLOCFORMATS = '=B16B'
BROADCASTLOCFORMATR = '2f3f' # x,y,roll,pitch,yaw
GETPOSITION = '\x04'
POSITIONFORMATS = '=B16B'
POSITIONFORMATR = '2f3f'
GETNUMALIVE = '\x05'
NUMALIVEFORMATS = '=B'
NUMALIVEFORMATR = '=B'
GETALIVE = '\x06'
ALIVEFORMATS = '=B16B'
ALIVEFORMATR = '=B'
SENDUPDATEOBJS = '\x07'
SENDUPDATEOBJSFORMAT = '=BB'
OBJSFORMAT = 'BBHH5f'			# objInfo struct
SENDFILE = '\x08'
SENDFILEFORMAT = '=BB'		# also need to send filename
SENDEND = '\x09'
SENDENDFORMAT = '=B16B'
SENDSTART = '\x10'
SENDSTARTFORMAT = '=B'
SENDACCEPT = '\x11'
SENDACCEPTFORMAT = '=B16B'
SENDGOBACK = '\x12'
SENDGOBACKFORMAT = '=B16B'

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
		print (BROADCASTIDSFORMATR % (ord(numIds)*16,));
		numToRead = calcsize(BROADCASTIDSFORMATR % (ord(numIds)*16,))
		print "numToRead:",numToRead
		reply = s.recv(numToRead*16);
		data = unpack(BROADCASTIDSFORMATR % (ord(numIds)*16,),reply);
	print "NumIds:",ord(numIds)
	print "IDs: ",data
	s.close()

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
	print "NumIds:",ord(reply)
	s.close()

