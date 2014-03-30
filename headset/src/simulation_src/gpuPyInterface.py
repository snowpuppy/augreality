# Filename: guiNetInterface.py
# Author: Thor Smith
# Purpose: Create functions for performing socket calls
#          to abstract the process of getting information
#

HOST = ''   # Symbolic name meaning the local host
PORT = 7778 # Arbitrary non-privileged port

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
OBJSGPUFORMAT = 'BBHH5f'			# objInfo struct

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

def sendUpdateObjsGpu(num,instId,typeShow,x2,y2,x3,y3,roll,pitch,yaw):
	# Set command
	# format SENDUPDATEOJBSGPUFORMAT'=BB' 
	# format OBJSGPUFORMAT 'BBHH5f'
	# Pack info
	command = SENDUPDATEOJBSGPU
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST,PORT))
	# send info
	s.send(command)
	s.send(chr(num))
	data = pack(OBJSGPUFORMAT, instId, typeShow, x2, y2, x3, y3, roll, pitch, yaw)
	s.send(data)
